//
// Copyright (c) 2008-2019 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "../Precompiled.h"

#include "../Core/Context.h"
#include "../Core/Mutex.h"
#include "../Core/Profiler.h"
#include "../Graphics/DebugRenderer.h"
#include "../Graphics/Model.h"
#include "../IO/Log.h"
#include "../Math/Ray.h"
#include "collision_shape_2.h"
#include "constraint_2.h"
#include "physics_events_2.h"
#include "physics_utils_2.h"
#include "physics_world_2.h"
#include "raycast_vehicle_2.h"
#include "rigid_body_2.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"

#include <Bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <Bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <Bullet/BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
#include <Bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <Bullet/BulletCollision/CollisionShapes/btSphereShape.h>
#include <Bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <Bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <Bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

extern ContactAddedCallback gContactAddedCallback;

namespace Urho3D
{

const char* PHYSICS_CATEGORY_2 = "Physics2";
extern const char* SUBSYSTEM_CATEGORY;

static const int MAX_SOLVER_ITERATIONS = 256;
static const Vector3 DEFAULT_GRAVITY = Vector3(0.0f, -9.81f, 0.0f);

PhysicsWorldConfig2 PhysicsWorld2::config;

static bool CompareRaycastResults(const PhysicsRaycastResult2& lhs, const PhysicsRaycastResult2& rhs)
{
    return lhs.distance_ < rhs.distance_;
}

void InternalPreTickCallback2(btDynamicsWorld* world, btScalar timeStep)
{
    static_cast<PhysicsWorld2*>(world->getWorldUserInfo())->PreStep(timeStep);
}

void InternalTickCallback2(btDynamicsWorld* world, btScalar timeStep)
{
    static_cast<PhysicsWorld2*>(world->getWorldUserInfo())->PostStep(timeStep);
}

static bool CustomMaterialCombinerCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0,
    int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
{
    // Ensure that shape type of colObj1Wrap is either btScaledBvhTriangleMeshShape or btBvhTriangleMeshShape
    // because btAdjustInternalEdgeContacts doesn't check types properly. Bug in the Bullet?
    const int shapeType = colObj1Wrap->getCollisionObject()->getCollisionShape()->getShapeType();
    if (shapeType == SCALED_TRIANGLE_MESH_SHAPE_PROXYTYPE || shapeType == TRIANGLE_SHAPE_PROXYTYPE
        || shapeType == MULTIMATERIAL_TRIANGLE_MESH_PROXYTYPE)
    {
        btAdjustInternalEdgeContacts(cp, colObj1Wrap, colObj0Wrap, partId1, index1);
    }

    cp.m_combinedFriction = colObj0Wrap->getCollisionObject()->getFriction() * colObj1Wrap->getCollisionObject()->getFriction();
    cp.m_combinedRestitution =
        colObj0Wrap->getCollisionObject()->getRestitution() * colObj1Wrap->getCollisionObject()->getRestitution();

    return true;
}

void RemoveCachedGeometryImpl(CollisionGeometryDataCache2& cache, Model* model)
{
    for (auto i = cache.Begin(); i != cache.End();)
    {
        auto current = i++;
        if (current->first_.first_ == model)
            cache.Erase(current);
    }
}

void CleanupGeometryCacheImpl(CollisionGeometryDataCache2& cache)
{
    for (auto i = cache.Begin(); i != cache.End();)
    {
        auto current = i++;
        if (current->second_.Refs() == 1)
            cache.Erase(current);
    }
}

/// Callback for physics world queries.
struct PhysicsQueryCallback : public btCollisionWorld::ContactResultCallback
{
    /// Construct.
    PhysicsQueryCallback(PODVector<RigidBody2*>& result, unsigned collisionMask) :
        result_(result),
        collisionMask_(collisionMask)
    {
    }

    /// Add a contact result.
    btScalar addSingleResult(btManifoldPoint&, const btCollisionObjectWrapper* colObj0Wrap, int, int,
        const btCollisionObjectWrapper* colObj1Wrap, int, int) override
    {
        auto* body = reinterpret_cast<RigidBody2*>(colObj0Wrap->getCollisionObject()->getUserPointer());
        if (body && !result_.Contains(body) && (body->GetCollisionLayer() & collisionMask_))
            result_.Push(body);
        body = reinterpret_cast<RigidBody2*>(colObj1Wrap->getCollisionObject()->getUserPointer());
        if (body && !result_.Contains(body) && (body->GetCollisionLayer() & collisionMask_))
            result_.Push(body);
        return 0.0f;
    }

    /// Found rigid bodies.
    PODVector<RigidBody2*>& result_;
    /// Collision mask for the query.
    unsigned collisionMask_;
};

PhysicsWorld2::PhysicsWorld2(Context* context) :
    Component(context),
    fps_(DEFAULT_FPS_2),
    debugMode_(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawConstraints | btIDebugDraw::DBG_DrawConstraintLimits)
{
    gContactAddedCallback = CustomMaterialCombinerCallback;

    if (PhysicsWorld2::config.collisionConfig_)
        collisionConfiguration_ = PhysicsWorld2::config.collisionConfig_;
    else
        collisionConfiguration_ = new btDefaultCollisionConfiguration();

    collisionDispatcher_ = new btCollisionDispatcher(collisionConfiguration_);
    btGImpactCollisionAlgorithm::registerAlgorithm(static_cast<btCollisionDispatcher*>(collisionDispatcher_.Get()));

    broadphase_ = new btDbvtBroadphase();
    solver_ = new btSequentialImpulseConstraintSolver();
    world_ = new btDiscreteDynamicsWorld(collisionDispatcher_.Get(), broadphase_.Get(), solver_.Get(), collisionConfiguration_);

    world_->setGravity(ToBtVector3(DEFAULT_GRAVITY));
    world_->getDispatchInfo().m_useContinuous = true;
    world_->getSolverInfo().m_splitImpulse = false; // Disable by default for performance
    world_->setDebugDrawer(this);
    world_->setInternalTickCallback(InternalPreTickCallback2, static_cast<void*>(this), true);
    world_->setInternalTickCallback(InternalTickCallback2, static_cast<void*>(this), false);
    world_->setSynchronizeAllMotionStates(true);
}

PhysicsWorld2::~PhysicsWorld2()
{
    if (scene_)
    {
        // Force all remaining constraints, rigid bodies and collision shapes to release themselves
        for (PODVector<Constraint2*>::Iterator i = constraints_.Begin(); i != constraints_.End(); ++i)
            (*i)->ReleaseConstraint();

        for (PODVector<RigidBody2*>::Iterator i = rigidBodies_.Begin(); i != rigidBodies_.End(); ++i)
            (*i)->ReleaseBody();

        for (PODVector<CollisionShape2*>::Iterator i = collisionShapes_.Begin(); i != collisionShapes_.End(); ++i)
            (*i)->ReleaseShape();
    }

    world_.Reset();
    solver_.Reset();
    broadphase_.Reset();
    collisionDispatcher_.Reset();

    // Delete configuration only if it was the default created by PhysicsWorld
    if (!PhysicsWorld2::config.collisionConfig_)
        delete collisionConfiguration_;
    collisionConfiguration_ = nullptr;
}

void PhysicsWorld2::RegisterObject(Context* context)
{
    context->RegisterFactory<PhysicsWorld2>(SUBSYSTEM_CATEGORY);

    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Gravity", GetGravity, SetGravity, Vector3, DEFAULT_GRAVITY, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Physics FPS", int, fps_, DEFAULT_FPS_2, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Max Substeps", int, maxSubSteps_, 0, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Solver Iterations", GetNumIterations, SetNumIterations, int, 10, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Net Max Angular Vel.", float, maxNetworkAngularVelocity_, DEFAULT_MAX_NETWORK_ANGULAR_VELOCITY_2, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Interpolation", bool, interpolation_, true, AM_FILE);
    URHO3D_ATTRIBUTE("Internal Edge Utility", bool, internalEdge_, true, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Split Impulse", GetSplitImpulse, SetSplitImpulse, bool, false, AM_DEFAULT);
}

bool PhysicsWorld2::isVisible(const btVector3& aabbMin, const btVector3& aabbMax)
{
    /*if (debugRenderer_)
        return debugRenderer_->IsInside(BoundingBox(ToVector3(aabbMin), ToVector3(aabbMax)));
    else
        return false;*/
    return true;
}

void PhysicsWorld2::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    if (debugRenderer_)
    {
        Node * n = GetNode();
        const Matrix3x4 m = n->GetWorldTransform();
        Vector3 f = ToVector3( from );
        f = m * f;
        Vector3 t = ToVector3( to );
        t = m * t;
        //debugRenderer_->AddLine(ToVector3(from), ToVector3(to), Color(color.x(), color.y(), color.z()), debugDepthTest_);
        debugRenderer_->AddLine( f, t, Color(color.x(), color.y(), color.z()), debugDepthTest_);
    }
}

void PhysicsWorld2::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
    if (debug)
    {
        URHO3D_PROFILE(PhysicsDrawDebug2);

        debugRenderer_ = debug;
        debugDepthTest_ = depthTest;
        world_->debugDrawWorld();
        debugRenderer_ = nullptr;
    }
}

void PhysicsWorld2::reportErrorWarning(const char* warningString)
{
    URHO3D_LOGWARNING("Physics: " + String(warningString));
}

void PhysicsWorld2::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime,
    const btVector3& color)
{
}

void PhysicsWorld2::draw3dText(const btVector3& location, const char* textString)
{
}

void PhysicsWorld2::Update(float timeStep)
{
    URHO3D_PROFILE(UpdatePhysics2);

    float internalTimeStep = 1.0f / fps_;
    int maxSubSteps = (int)(timeStep * fps_) + 1;
    if (maxSubSteps_ < 0)
    {
        internalTimeStep = timeStep;
        maxSubSteps = 1;
    }
    else if (maxSubSteps_ > 0)
        maxSubSteps = Min(maxSubSteps, maxSubSteps_);

    delayedWorldTransforms_.Clear();
    simulating_ = true;

    if (interpolation_)
        world_->stepSimulation(timeStep, maxSubSteps, internalTimeStep);
    else
    {
        timeAcc_ += timeStep;
        while (timeAcc_ >= internalTimeStep && maxSubSteps > 0)
        {
            world_->stepSimulation(internalTimeStep, 0, internalTimeStep);
            timeAcc_ -= internalTimeStep;
            --maxSubSteps;
        }
    }

    simulating_ = false;

    // Apply delayed (parented) world transforms now
    while (!delayedWorldTransforms_.Empty())
    {
        for (HashMap<RigidBody2*, DelayedWorldTransform2>::Iterator i = delayedWorldTransforms_.Begin();
             i != delayedWorldTransforms_.End();)
        {
            const DelayedWorldTransform2 & transform = i->second_;

            // If parent's transform has already been assigned, can proceed
            if (!delayedWorldTransforms_.Contains(transform.parentRigidBody_))
            {
                transform.rigidBody_->ApplyWorldTransform(transform.worldPosition_, transform.worldRotation_);
                i = delayedWorldTransforms_.Erase(i);
            }
            else
                ++i;
        }
    }
}

void PhysicsWorld2::UpdateCollisions()
{
    world_->performDiscreteCollisionDetection();
}

void PhysicsWorld2::SetFps(int fps)
{
    fps_ = (unsigned)Clamp(fps, 1, 1000);

    MarkNetworkUpdate();
}

void PhysicsWorld2::SetGravity(const Vector3& gravity)
{
    world_->setGravity(ToBtVector3(gravity));

    MarkNetworkUpdate();
}

void PhysicsWorld2::SetMaxSubSteps(int num)
{
    maxSubSteps_ = num;
    MarkNetworkUpdate();
}

void PhysicsWorld2::SetNumIterations(int num)
{
    num = Clamp(num, 1, MAX_SOLVER_ITERATIONS);
    world_->getSolverInfo().m_numIterations = num;

    MarkNetworkUpdate();
}

void PhysicsWorld2::SetUpdateEnabled(bool enable)
{
    updateEnabled_ = enable;
}

void PhysicsWorld2::SetInterpolation(bool enable)
{
    interpolation_ = enable;
}

void PhysicsWorld2::SetInternalEdge(bool enable)
{
    internalEdge_ = enable;

    MarkNetworkUpdate();
}

void PhysicsWorld2::SetSplitImpulse(bool enable)
{
    world_->getSolverInfo().m_splitImpulse = enable;

    MarkNetworkUpdate();
}

void PhysicsWorld2::SetMaxNetworkAngularVelocity(float velocity)
{
    maxNetworkAngularVelocity_ = Clamp(velocity, 1.0f, 32767.0f);

    MarkNetworkUpdate();
}

void PhysicsWorld2::Raycast(PODVector<PhysicsRaycastResult2>& result, const Ray& ray, float maxDistance, unsigned collisionMask)
{
    URHO3D_PROFILE(PhysicsRaycast2);

    if (maxDistance >= M_INFINITY)
        URHO3D_LOGWARNING("Infinite maxDistance in physics raycast is not supported");

    btCollisionWorld::AllHitsRayResultCallback
        rayCallback(ToBtVector3(ray.origin_), ToBtVector3(ray.origin_ + maxDistance * ray.direction_));
    rayCallback.m_collisionFilterGroup = (short)0xffff;
    rayCallback.m_collisionFilterMask = (short)collisionMask;

    world_->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);

    for (int i = 0; i < rayCallback.m_collisionObjects.size(); ++i)
    {
        PhysicsRaycastResult2 newResult;
        newResult.body_ = static_cast<RigidBody2*>(rayCallback.m_collisionObjects[i]->getUserPointer());
        newResult.position_ = ToVector3(rayCallback.m_hitPointWorld[i]);
        newResult.normal_ = ToVector3(rayCallback.m_hitNormalWorld[i]);
        newResult.distance_ = (newResult.position_ - ray.origin_).Length();
        newResult.hitFraction_ = rayCallback.m_closestHitFraction;
        result.Push(newResult);
    }

    Sort(result.Begin(), result.End(), CompareRaycastResults);
}

void PhysicsWorld2::RaycastSingle(PhysicsRaycastResult2& result, const Ray& ray, float maxDistance, unsigned collisionMask)
{
    URHO3D_PROFILE(PhysicsRaycastSingle2);

    if (maxDistance >= M_INFINITY)
        URHO3D_LOGWARNING("Infinite maxDistance in physics raycast is not supported");

    btCollisionWorld::ClosestRayResultCallback
        rayCallback(ToBtVector3(ray.origin_), ToBtVector3(ray.origin_ + maxDistance * ray.direction_));
    rayCallback.m_collisionFilterGroup = (short)0xffff;
    rayCallback.m_collisionFilterMask = (short)collisionMask;

    world_->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);

    if (rayCallback.hasHit())
    {
        result.position_ = ToVector3(rayCallback.m_hitPointWorld);
        result.normal_ = ToVector3(rayCallback.m_hitNormalWorld);
        result.distance_ = (result.position_ - ray.origin_).Length();
        result.hitFraction_ = rayCallback.m_closestHitFraction;
        result.body_ = static_cast<RigidBody2*>(rayCallback.m_collisionObject->getUserPointer());
    }
    else
    {
        result.position_ = Vector3::ZERO;
        result.normal_ = Vector3::ZERO;
        result.distance_ = M_INFINITY;
        result.hitFraction_ = 0.0f;
        result.body_ = nullptr;
    }
}

void PhysicsWorld2::RaycastSingleSegmented(PhysicsRaycastResult2& result, const Ray& ray, float maxDistance, float segmentDistance, unsigned collisionMask, float overlapDistance)
{
    URHO3D_PROFILE(PhysicsRaycastSingleSegmented2);

    assert(overlapDistance < segmentDistance);

    if (maxDistance >= M_INFINITY)
        URHO3D_LOGWARNING("Infinite maxDistance in physics raycast is not supported");

    const btVector3 direction = ToBtVector3(ray.direction_);
    const auto count = CeilToInt(maxDistance / segmentDistance);

    btVector3 start = ToBtVector3(ray.origin_);
    // overlap a bit with the previous segment for better precision, to avoid missing hits
    const btVector3 overlap = direction * overlapDistance;
    float remainingDistance = maxDistance;

    for (auto i = 0; i < count; ++i)
    {
        const float distance = Min(remainingDistance, segmentDistance); // The last segment may be shorter
        const btVector3 end = start + distance * direction;

        btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
        rayCallback.m_collisionFilterGroup = (short)0xffff;
        rayCallback.m_collisionFilterMask = (short)collisionMask;

        world_->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);

        if (rayCallback.hasHit())
        {
            result.position_ = ToVector3(rayCallback.m_hitPointWorld);
            result.normal_ = ToVector3(rayCallback.m_hitNormalWorld);
            result.distance_ = (result.position_ - ray.origin_).Length();
            result.hitFraction_ = rayCallback.m_closestHitFraction;
            result.body_ = static_cast<RigidBody2*>(rayCallback.m_collisionObject->getUserPointer());
            // No need to cast the rest of the segments
            return;
        }

        // Use the end position as the new start position
        start = end - overlap;
        remainingDistance -= segmentDistance;
    }

    // Didn't hit anything
    result.position_ = Vector3::ZERO;
    result.normal_ = Vector3::ZERO;
    result.distance_ = M_INFINITY;
    result.hitFraction_ = 0.0f;
    result.body_ = nullptr;
}

void PhysicsWorld2::SphereCast(PhysicsRaycastResult2& result, const Ray& ray, float radius, float maxDistance, unsigned collisionMask)
{
    URHO3D_PROFILE(PhysicsSphereCast2);

    if (maxDistance >= M_INFINITY)
        URHO3D_LOGWARNING("Infinite maxDistance in physics sphere cast is not supported");

    btSphereShape shape(radius);
    Vector3 endPos = ray.origin_ + maxDistance * ray.direction_;

    btCollisionWorld::ClosestConvexResultCallback
        convexCallback(ToBtVector3(ray.origin_), ToBtVector3(endPos));
    convexCallback.m_collisionFilterGroup = (short)0xffff;
    convexCallback.m_collisionFilterMask = (short)collisionMask;

    world_->convexSweepTest(&shape, btTransform(btQuaternion::getIdentity(), convexCallback.m_convexFromWorld),
        btTransform(btQuaternion::getIdentity(), convexCallback.m_convexToWorld), convexCallback);

    if (convexCallback.hasHit())
    {
        result.body_ = static_cast<RigidBody2*>(convexCallback.m_hitCollisionObject->getUserPointer());
        result.position_ = ToVector3(convexCallback.m_hitPointWorld);
        result.normal_ = ToVector3(convexCallback.m_hitNormalWorld);
        result.distance_ = convexCallback.m_closestHitFraction * (endPos - ray.origin_).Length();
        result.hitFraction_ = convexCallback.m_closestHitFraction;
    }
    else
    {
        result.body_ = nullptr;
        result.position_ = Vector3::ZERO;
        result.normal_ = Vector3::ZERO;
        result.distance_ = M_INFINITY;
        result.hitFraction_ = 0.0f;
    }
}

void PhysicsWorld2::ConvexCast(PhysicsRaycastResult2& result, CollisionShape2* shape, const Vector3& startPos,
    const Quaternion& startRot, const Vector3& endPos, const Quaternion& endRot, unsigned collisionMask)
{
    if (!shape || !shape->GetCollisionShape())
    {
        URHO3D_LOGERROR("Null collision shape for convex cast");
        result.body_ = nullptr;
        result.position_ = Vector3::ZERO;
        result.normal_ = Vector3::ZERO;
        result.distance_ = M_INFINITY;
        result.hitFraction_ = 0.0f;
        return;
    }

    // If shape is attached in a rigidbody, set its collision group temporarily to 0 to make sure it is not returned in the sweep result
    auto* bodyComp = shape->GetComponent<RigidBody2>();
    btRigidBody* body = bodyComp ? bodyComp->GetBody() : nullptr;
    btBroadphaseProxy* proxy = body ? body->getBroadphaseProxy() : nullptr;
    short group = 0;
    if (proxy)
    {
        group = proxy->m_collisionFilterGroup;
        proxy->m_collisionFilterGroup = 0;
    }

    // Take the shape's offset position & rotation into account
    Node* shapeNode = shape->GetNode();
    Matrix3x4 startTransform(startPos, startRot, shapeNode ? shapeNode->GetScale() : Vector3::ONE);
    Matrix3x4 endTransform(endPos, endRot, shapeNode ? shapeNode->GetScale() : Vector3::ONE);
    Vector3 effectiveStartPos = startTransform * shape->GetPosition();
    Vector3 effectiveEndPos = endTransform * shape->GetPosition();
    Quaternion effectiveStartRot = startRot * shape->GetRotation();
    Quaternion effectiveEndRot = endRot * shape->GetRotation();

    ConvexCast(result, shape->GetCollisionShape(), effectiveStartPos, effectiveStartRot, effectiveEndPos, effectiveEndRot, collisionMask);

    // Restore the collision group
    if (proxy)
        proxy->m_collisionFilterGroup = group;
}

void PhysicsWorld2::ConvexCast(PhysicsRaycastResult2& result, btCollisionShape* shape, const Vector3& startPos,
    const Quaternion& startRot, const Vector3& endPos, const Quaternion& endRot, unsigned collisionMask)
{
    if (!shape)
    {
        URHO3D_LOGERROR("Null collision shape for convex cast");
        result.body_ = nullptr;
        result.position_ = Vector3::ZERO;
        result.normal_ = Vector3::ZERO;
        result.distance_ = M_INFINITY;
        result.hitFraction_ = 0.0f;
        return;
    }

    if (!shape->isConvex())
    {
        URHO3D_LOGERROR("Can not use non-convex collision shape for convex cast");
        result.body_ = nullptr;
        result.position_ = Vector3::ZERO;
        result.normal_ = Vector3::ZERO;
        result.distance_ = M_INFINITY;
        result.hitFraction_ = 0.0f;
        return;
    }

    URHO3D_PROFILE(PhysicsConvexCast);

    btCollisionWorld::ClosestConvexResultCallback convexCallback(ToBtVector3(startPos), ToBtVector3(endPos));
    convexCallback.m_collisionFilterGroup = (short)0xffff;
    convexCallback.m_collisionFilterMask = (short)collisionMask;

    world_->convexSweepTest(static_cast<btConvexShape*>(shape), btTransform(ToBtQuaternion(startRot),
            convexCallback.m_convexFromWorld), btTransform(ToBtQuaternion(endRot), convexCallback.m_convexToWorld),
        convexCallback);

    if (convexCallback.hasHit())
    {
        result.body_ = static_cast<RigidBody2*>(convexCallback.m_hitCollisionObject->getUserPointer());
        result.position_ = ToVector3(convexCallback.m_hitPointWorld);
        result.normal_ = ToVector3(convexCallback.m_hitNormalWorld);
        result.distance_ = convexCallback.m_closestHitFraction * (endPos - startPos).Length();
        result.hitFraction_ = convexCallback.m_closestHitFraction;
    }
    else
    {
        result.body_ = nullptr;
        result.position_ = Vector3::ZERO;
        result.normal_ = Vector3::ZERO;
        result.distance_ = M_INFINITY;
        result.hitFraction_ = 0.0f;
    }
}

void PhysicsWorld2::RemoveCachedGeometry(Model* model)
{
    RemoveCachedGeometryImpl(triMeshCache_, model);
    RemoveCachedGeometryImpl(convexCache_, model);
    RemoveCachedGeometryImpl(gimpactTrimeshCache_, model);
}

void PhysicsWorld2::GetRigidBodies(PODVector<RigidBody2*>& result, const Sphere& sphere, unsigned collisionMask)
{
    URHO3D_PROFILE(PhysicsSphereQuery);

    result.Clear();

    btSphereShape sphereShape(sphere.radius_);
    UniquePtr<btRigidBody> tempRigidBody(new btRigidBody(1.0f, nullptr, &sphereShape));
    tempRigidBody->setWorldTransform(btTransform(btQuaternion::getIdentity(), ToBtVector3(sphere.center_)));
    // Need to activate the temporary rigid body to get reliable results from static, sleeping objects
    tempRigidBody->activate();
    world_->addRigidBody(tempRigidBody.Get());

    PhysicsQueryCallback callback(result, collisionMask);
    world_->contactTest(tempRigidBody.Get(), callback);

    world_->removeRigidBody(tempRigidBody.Get());
}

void PhysicsWorld2::GetRigidBodies(PODVector<RigidBody2*>& result, const BoundingBox& box, unsigned collisionMask)
{
    URHO3D_PROFILE(PhysicsBoxQuery);

    result.Clear();

    btBoxShape boxShape(ToBtVector3(box.HalfSize()));
    UniquePtr<btRigidBody> tempRigidBody(new btRigidBody(1.0f, nullptr, &boxShape));
    tempRigidBody->setWorldTransform(btTransform(btQuaternion::getIdentity(), ToBtVector3(box.Center())));
    tempRigidBody->activate();
    world_->addRigidBody(tempRigidBody.Get());

    PhysicsQueryCallback callback(result, collisionMask);
    world_->contactTest(tempRigidBody.Get(), callback);

    world_->removeRigidBody(tempRigidBody.Get());
}

void PhysicsWorld2::GetRigidBodies(PODVector<RigidBody2*>& result, const RigidBody2* body)
{
    URHO3D_PROFILE(PhysicsBodyQuery);

    result.Clear();

    if (!body || !body->GetBody())
        return;

    PhysicsQueryCallback callback(result, body->GetCollisionMask());
    world_->contactTest(body->GetBody(), callback);

    // Remove the body itself from the returned list
    for (unsigned i = 0; i < result.Size(); i++)
    {
        if (result[i] == body)
        {
            result.Erase(i);
            break;
        }
    }
}

void PhysicsWorld2::GetCollidingBodies(PODVector<RigidBody2*>& result, const RigidBody2* body)
{
    URHO3D_PROFILE(GetCollidingBodies);

    result.Clear();

    for (HashMap<Pair<WeakPtr<RigidBody2>, WeakPtr<RigidBody2> >, ManifoldPair2>::Iterator i = currentCollisions_.Begin();
         i != currentCollisions_.End(); ++i)
    {
        if (i->first_.first_ == body)
        {
            if (i->first_.second_)
                result.Push(i->first_.second_);
        }
        else if (i->first_.second_ == body)
        {
            if (i->first_.first_)
                result.Push(i->first_.first_);
        }
    }
}

Vector3 PhysicsWorld2::GetGravity() const
{
    return ToVector3(world_->getGravity());
}

int PhysicsWorld2::GetNumIterations() const
{
    return world_->getSolverInfo().m_numIterations;
}

bool PhysicsWorld2::GetSplitImpulse() const
{
    return world_->getSolverInfo().m_splitImpulse != 0;
}

void PhysicsWorld2::AddRigidBody(RigidBody2* body)
{
    rigidBodies_.Push(body);
}

void PhysicsWorld2::RemoveRigidBody(RigidBody2* body)
{
    rigidBodies_.Remove(body);
    // Remove possible dangling pointer from the delayedWorldTransforms structure
    delayedWorldTransforms_.Erase(body);
}

void PhysicsWorld2::AddCollisionShape(CollisionShape2* shape)
{
    collisionShapes_.Push(shape);
}

void PhysicsWorld2::RemoveCollisionShape(CollisionShape2* shape)
{
    collisionShapes_.Remove(shape);
}

void PhysicsWorld2::AddConstraint(Constraint2* constraint)
{
    constraints_.Push(constraint);
}

void PhysicsWorld2::RemoveConstraint(Constraint2* constraint)
{
    constraints_.Remove(constraint);
}

void PhysicsWorld2::AddDelayedWorldTransform(const DelayedWorldTransform2& transform)
{
    delayedWorldTransforms_[transform.rigidBody_] = transform;
}

void PhysicsWorld2::DrawDebugGeometry(bool depthTest)
{
    Scene * s = GetScene();
    auto* debug = s->GetComponent<DebugRenderer>();
    DrawDebugGeometry(debug, depthTest);
}

void PhysicsWorld2::SetDebugRenderer(DebugRenderer* debug)
{
    debugRenderer_ = debug;
}

void PhysicsWorld2::SetDebugDepthTest(bool enable)
{
    debugDepthTest_ = enable;
}

void PhysicsWorld2::CleanupGeometryCache()
{
    // Remove cached shapes whose only reference is the cache itself
    CleanupGeometryCacheImpl(triMeshCache_);
    CleanupGeometryCacheImpl(convexCache_);
    CleanupGeometryCacheImpl(gimpactTrimeshCache_);
}

void PhysicsWorld2::OnSceneSet(Scene* scene)
{
    // Subscribe to the scene subsystem update, which will trigger the physics simulation step
    if (scene)
    {
        scene_ = GetScene();
        SubscribeToEvent(scene_, E_SCENESUBSYSTEMUPDATE, URHO3D_HANDLER(PhysicsWorld2, HandleSceneSubsystemUpdate));
    }
    else
        UnsubscribeFromEvent(E_SCENESUBSYSTEMUPDATE);
}

void PhysicsWorld2::HandleSceneSubsystemUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!updateEnabled_)
        return;

    using namespace SceneSubsystemUpdate;
    Update(eventData[P_TIMESTEP].GetFloat());
}

void PhysicsWorld2::PreStep(float timeStep)
{
    // Send pre-step event
    using namespace PhysicsPreStep2;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_WORLD] = this;
    eventData[P_TIMESTEP] = timeStep;
    SendEvent(E_PHYSICSPRESTEP_2, eventData);

    // Start profiling block for the actual simulation step
#ifdef URHO3D_PROFILING
    auto* profiler = GetSubsystem<Profiler>();
    if (profiler)
        profiler->BeginBlock("StepSimulation");
#endif
}

void PhysicsWorld2::PostStep(float timeStep)
{
#ifdef URHO3D_PROFILING
    auto* profiler = GetSubsystem<Profiler>();
    if (profiler)
        profiler->EndBlock();
#endif

    SendCollisionEvents();

    // Send post-step event
    using namespace PhysicsPostStep2;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_WORLD] = this;
    eventData[P_TIMESTEP] = timeStep;
    SendEvent(E_PHYSICSPOSTSTEP_2, eventData);
}

void PhysicsWorld2::SendCollisionEvents()
{
    URHO3D_PROFILE(SendCollisionEvents);

    currentCollisions_.Clear();
    physicsCollisionData_.Clear();
    nodeCollisionData_.Clear();

    int numManifolds = collisionDispatcher_->getNumManifolds();

    if (numManifolds)
    {
        physicsCollisionData_[PhysicsCollision2::P_WORLD] = this;

        for (int i = 0; i < numManifolds; ++i)
        {
            btPersistentManifold* contactManifold = collisionDispatcher_->getManifoldByIndexInternal(i);
            // First check that there are actual contacts, as the manifold exists also when objects are close but not touching
            if (!contactManifold->getNumContacts())
                continue;

            const btCollisionObject* objectA = contactManifold->getBody0();
            const btCollisionObject* objectB = contactManifold->getBody1();

            auto* bodyA = static_cast<RigidBody2*>(objectA->getUserPointer());
            auto* bodyB = static_cast<RigidBody2*>(objectB->getUserPointer());
            // If it's not a rigidbody, maybe a ghost object
            if (!bodyA || !bodyB)
                continue;

            // Skip collision event signaling if both objects are static, or if collision event mode does not match
            if (bodyA->GetMass() == 0.0f && bodyB->GetMass() == 0.0f)
                continue;
            if (bodyA->GetCollisionEventMode() == COLLISION_NEVER_2 || bodyB->GetCollisionEventMode() == COLLISION_NEVER_2)
                continue;
            if (bodyA->GetCollisionEventMode() == COLLISION_ACTIVE_2 && bodyB->GetCollisionEventMode() == COLLISION_ACTIVE_2 &&
                !bodyA->IsActive() && !bodyB->IsActive())
                continue;

            WeakPtr<RigidBody2> bodyWeakA(bodyA);
            WeakPtr<RigidBody2> bodyWeakB(bodyB);

            // First only store the collision pair as weak pointers and the manifold pointer, so user code can safely destroy
            // objects during collision event handling
            Pair<WeakPtr<RigidBody2>, WeakPtr<RigidBody2> > bodyPair;
            if (bodyA < bodyB)
            {
                bodyPair = MakePair(bodyWeakA, bodyWeakB);
                currentCollisions_[bodyPair].manifold_ = contactManifold;
            }
            else
            {
                bodyPair = MakePair(bodyWeakB, bodyWeakA);
                currentCollisions_[bodyPair].flippedManifold_ = contactManifold;
            }
        }

        for (HashMap<Pair<WeakPtr<RigidBody2>, WeakPtr<RigidBody2> >, ManifoldPair2>::Iterator i = currentCollisions_.Begin();
             i != currentCollisions_.End(); ++i)
        {
            RigidBody2* bodyA = i->first_.first_;
            RigidBody2* bodyB = i->first_.second_;
            if (!bodyA || !bodyB)
                continue;

            Node* nodeA = bodyA->GetNode();
            Node* nodeB = bodyB->GetNode();
            WeakPtr<Node> nodeWeakA(nodeA);
            WeakPtr<Node> nodeWeakB(nodeB);

            bool trigger = bodyA->IsTrigger() || bodyB->IsTrigger();
            bool newCollision = !previousCollisions_.Contains(i->first_);

            physicsCollisionData_[PhysicsCollision2::P_NODEA] = nodeA;
            physicsCollisionData_[PhysicsCollision2::P_NODEB] = nodeB;
            physicsCollisionData_[PhysicsCollision2::P_BODYA] = bodyA;
            physicsCollisionData_[PhysicsCollision2::P_BODYB] = bodyB;
            physicsCollisionData_[PhysicsCollision2::P_TRIGGER] = trigger;

            contacts_.Clear();

            // "Pointers not flipped"-manifold, send unmodified normals
            btPersistentManifold* contactManifold = i->second_.manifold_;
            if (contactManifold)
            {
                for (int j = 0; j < contactManifold->getNumContacts(); ++j)
                {
                    btManifoldPoint& point = contactManifold->getContactPoint(j);
                    contacts_.WriteVector3(ToVector3(point.m_positionWorldOnB));
                    contacts_.WriteVector3(ToVector3(point.m_normalWorldOnB));
                    contacts_.WriteFloat(point.m_distance1);
                    contacts_.WriteFloat(point.m_appliedImpulse);
                }
            }
            // "Pointers flipped"-manifold, flip normals also
            contactManifold = i->second_.flippedManifold_;
            if (contactManifold)
            {
                for (int j = 0; j < contactManifold->getNumContacts(); ++j)
                {
                    btManifoldPoint& point = contactManifold->getContactPoint(j);
                    contacts_.WriteVector3(ToVector3(point.m_positionWorldOnB));
                    contacts_.WriteVector3(-ToVector3(point.m_normalWorldOnB));
                    contacts_.WriteFloat(point.m_distance1);
                    contacts_.WriteFloat(point.m_appliedImpulse);
                }
            }

            physicsCollisionData_[PhysicsCollision2::P_CONTACTS] = contacts_.GetBuffer();

            // Send separate collision start event if collision is new
            if (newCollision)
            {
                SendEvent(E_PHYSICSCOLLISIONSTART_2, physicsCollisionData_);
                // Skip rest of processing if either of the nodes or bodies is removed as a response to the event
                if (!nodeWeakA || !nodeWeakB || !i->first_.first_ || !i->first_.second_)
                    continue;
            }

            // Then send the ongoing collision event
            SendEvent(E_PHYSICSCOLLISION_2, physicsCollisionData_);
            if (!nodeWeakA || !nodeWeakB || !i->first_.first_ || !i->first_.second_)
                continue;

            nodeCollisionData_[NodeCollision2::P_BODY] = bodyA;
            nodeCollisionData_[NodeCollision2::P_OTHERNODE] = nodeB;
            nodeCollisionData_[NodeCollision2::P_OTHERBODY] = bodyB;
            nodeCollisionData_[NodeCollision2::P_TRIGGER] = trigger;
            nodeCollisionData_[NodeCollision2::P_CONTACTS] = contacts_.GetBuffer();

            if (newCollision)
            {
                nodeA->SendEvent(E_NODECOLLISIONSTART_2, nodeCollisionData_);
                if (!nodeWeakA || !nodeWeakB || !i->first_.first_ || !i->first_.second_)
                    continue;
            }

            nodeA->SendEvent(E_NODECOLLISION_2, nodeCollisionData_);
            if (!nodeWeakA || !nodeWeakB || !i->first_.first_ || !i->first_.second_)
                continue;

            // Flip perspective to body B
            contacts_.Clear();
            contactManifold = i->second_.manifold_;
            if (contactManifold)
            {
                for (int j = 0; j < contactManifold->getNumContacts(); ++j)
                {
                    btManifoldPoint& point = contactManifold->getContactPoint(j);
                    contacts_.WriteVector3(ToVector3(point.m_positionWorldOnB));
                    contacts_.WriteVector3(-ToVector3(point.m_normalWorldOnB));
                    contacts_.WriteFloat(point.m_distance1);
                    contacts_.WriteFloat(point.m_appliedImpulse);
                }
            }
            contactManifold = i->second_.flippedManifold_;
            if (contactManifold)
            {
                for (int j = 0; j < contactManifold->getNumContacts(); ++j)
                {
                    btManifoldPoint& point = contactManifold->getContactPoint(j);
                    contacts_.WriteVector3(ToVector3(point.m_positionWorldOnB));
                    contacts_.WriteVector3(ToVector3(point.m_normalWorldOnB));
                    contacts_.WriteFloat(point.m_distance1);
                    contacts_.WriteFloat(point.m_appliedImpulse);
                }
            }

            nodeCollisionData_[NodeCollision2::P_BODY] = bodyB;
            nodeCollisionData_[NodeCollision2::P_OTHERNODE] = nodeA;
            nodeCollisionData_[NodeCollision2::P_OTHERBODY] = bodyA;
            nodeCollisionData_[NodeCollision2::P_CONTACTS] = contacts_.GetBuffer();

            if (newCollision)
            {
                nodeB->SendEvent(E_NODECOLLISIONSTART_2, nodeCollisionData_);
                if (!nodeWeakA || !nodeWeakB || !i->first_.first_ || !i->first_.second_)
                    continue;
            }

            nodeB->SendEvent(E_NODECOLLISION_2, nodeCollisionData_);
        }
    }

    // Send collision end events as applicable
    {
        physicsCollisionData_[PhysicsCollisionEnd2::P_WORLD] = this;

        for (HashMap<Pair<WeakPtr<RigidBody2>, WeakPtr<RigidBody2> >, ManifoldPair2>::Iterator
                 i = previousCollisions_.Begin(); i != previousCollisions_.End(); ++i)
        {
            if (!currentCollisions_.Contains(i->first_))
            {
                RigidBody2* bodyA = i->first_.first_;
                RigidBody2* bodyB = i->first_.second_;
                if (!bodyA || !bodyB)
                    continue;

                bool trigger = bodyA->IsTrigger() || bodyB->IsTrigger();

                // Skip collision event signaling if both objects are static, or if collision event mode does not match
                if (bodyA->GetMass() == 0.0f && bodyB->GetMass() == 0.0f)
                    continue;
                if (bodyA->GetCollisionEventMode() == COLLISION_NEVER_2 || bodyB->GetCollisionEventMode() == COLLISION_NEVER_2)
                    continue;
                if (bodyA->GetCollisionEventMode() == COLLISION_ACTIVE_2 && bodyB->GetCollisionEventMode() == COLLISION_ACTIVE_2 &&
                    !bodyA->IsActive() && !bodyB->IsActive())
                    continue;

                Node* nodeA = bodyA->GetNode();
                Node* nodeB = bodyB->GetNode();
                WeakPtr<Node> nodeWeakA(nodeA);
                WeakPtr<Node> nodeWeakB(nodeB);

                physicsCollisionData_[PhysicsCollisionEnd2::P_BODYA] = bodyA;
                physicsCollisionData_[PhysicsCollisionEnd2::P_BODYB] = bodyB;
                physicsCollisionData_[PhysicsCollisionEnd2::P_NODEA] = nodeA;
                physicsCollisionData_[PhysicsCollisionEnd2::P_NODEB] = nodeB;
                physicsCollisionData_[PhysicsCollisionEnd2::P_TRIGGER] = trigger;

                SendEvent(E_PHYSICSCOLLISIONEND_2, physicsCollisionData_);
                // Skip rest of processing if either of the nodes or bodies is removed as a response to the event
                if (!nodeWeakA || !nodeWeakB || !i->first_.first_ || !i->first_.second_)
                    continue;

                nodeCollisionData_[NodeCollisionEnd2::P_BODY] = bodyA;
                nodeCollisionData_[NodeCollisionEnd2::P_OTHERNODE] = nodeB;
                nodeCollisionData_[NodeCollisionEnd2::P_OTHERBODY] = bodyB;
                nodeCollisionData_[NodeCollisionEnd2::P_TRIGGER] = trigger;

                nodeA->SendEvent(E_NODECOLLISIONEND_2, nodeCollisionData_);
                if (!nodeWeakA || !nodeWeakB || !i->first_.first_ || !i->first_.second_)
                    continue;

                nodeCollisionData_[NodeCollisionEnd2::P_BODY] = bodyB;
                nodeCollisionData_[NodeCollisionEnd2::P_OTHERNODE] = nodeA;
                nodeCollisionData_[NodeCollisionEnd2::P_OTHERBODY] = bodyA;

                nodeB->SendEvent(E_NODECOLLISIONEND_2, nodeCollisionData_);
            }
        }
    }

    previousCollisions_ = currentCollisions_;
}

PhysicsWorld2 * PhysicsWorld2::getWorld( Node * node )
{
    if ( !node )
        return nullptr;

    Node * pn = node->GetParent();
    PhysicsWorld2 * w2 = pn->GetComponent<PhysicsWorld2>();
    return w2;
}

void RegisterPhysicsLibrary2(Context* context)
{
    // My modification.
    context->RegisterFactory<PhysicsWorld2>();

    CollisionShape2::RegisterObject(context);
    RigidBody2::RegisterObject(context);
    Constraint2::RegisterObject(context);
    PhysicsWorld2::RegisterObject(context);
    RaycastVehicle2::RegisterObject(context);
}

}
