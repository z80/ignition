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
#include "../Core/Profiler.h"
#include "../IO/Log.h"
#include "../IO/MemoryBuffer.h"
#include "collision_shape_2.h"
#include "constraint_2.h"
#include "physics_utils_2.h"
#include "physics_world_2.h"
#include "rigid_body_2.h"
#include "../Resource/ResourceCache.h"
#include "../Resource/ResourceEvents.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"
#include "../Scene/SmoothedTransform.h"

#include <Bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <Bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include <Bullet/BulletCollision/CollisionShapes/btCompoundShape.h>

namespace Urho3D
{

static const float DEFAULT_MASS_2 = 0.0f;
static const float DEFAULT_FRICTION_2 = 0.5f;
static const float DEFAULT_RESTITUTION_2 = 0.0f;
static const float DEFAULT_ROLLING_FRICTION_2 = 0.0f;
static const unsigned DEFAULT_COLLISION_LAYER_2 = 0x1;
static const unsigned DEFAULT_COLLISION_MASK_2 = M_MAX_UNSIGNED;

static const char* collisionEventModeNames2[] =
{
    "Never",
    "When Active",
    "Always",
    nullptr
};

extern const char* PHYSICS_CATEGORY_2;

RigidBody2::RigidBody2(Context* context) :
    Component(context),
    gravityOverride_(Vector3::ZERO),
    centerOfMass_(Vector3::ZERO),
    mass_(DEFAULT_MASS_2),
    collisionLayer_(DEFAULT_COLLISION_LAYER_2),
    collisionMask_(DEFAULT_COLLISION_MASK_2),
    collisionEventMode_(COLLISION_ACTIVE_2),
    lastPosition_(Vector3::ZERO),
    lastRotation_(Quaternion::IDENTITY),
    kinematic_(false),
    trigger_(false),
    useGravity_(true),
    readdBody_(false),
    inWorld_(false),
    enableMassUpdate_(true),
    hasSimulated_(false)
{
    compoundShape_ = new btCompoundShape();
    shiftedCompoundShape_ = new btCompoundShape();
}

RigidBody2::~RigidBody2()
{
    ReleaseBody();

    if (physicsWorld_)
        physicsWorld_->RemoveRigidBody(this);
}

void RigidBody2::RegisterObject(Context* context)
{
    context->RegisterFactory<RigidBody2>(PHYSICS_CATEGORY_2);

    URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Physics Rotation", GetRotation, SetRotation, Quaternion, Quaternion::IDENTITY, AM_FILE | AM_NOEDIT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Physics Position", GetPosition, SetPosition, Vector3, Vector3::ZERO, AM_FILE | AM_NOEDIT);
    URHO3D_ATTRIBUTE_EX("Mass", float, mass_, MarkBodyDirty, DEFAULT_MASS_2, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Friction", GetFriction, SetFriction, float, DEFAULT_FRICTION_2, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Anisotropic Friction", GetAnisotropicFriction, SetAnisotropicFriction, Vector3, Vector3::ONE,
        AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Rolling Friction", GetRollingFriction, SetRollingFriction, float, DEFAULT_ROLLING_FRICTION_2, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Restitution", GetRestitution, SetRestitution, float, DEFAULT_RESTITUTION_2, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Linear Velocity", GetLinearVelocity, SetLinearVelocity, Vector3, Vector3::ZERO,
        AM_DEFAULT | AM_LATESTDATA);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Angular Velocity", GetAngularVelocity, SetAngularVelocity, Vector3, Vector3::ZERO, AM_FILE);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Linear Factor", GetLinearFactor, SetLinearFactor, Vector3, Vector3::ONE, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Angular Factor", GetAngularFactor, SetAngularFactor, Vector3, Vector3::ONE, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Linear Damping", GetLinearDamping, SetLinearDamping, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Angular Damping", GetAngularDamping, SetAngularDamping, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Linear Rest Threshold", GetLinearRestThreshold, SetLinearRestThreshold, float, 0.8f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Angular Rest Threshold", GetAngularRestThreshold, SetAngularRestThreshold, float, 1.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE_EX("Collision Layer", int, collisionLayer_, MarkBodyDirty, DEFAULT_COLLISION_LAYER_2, AM_DEFAULT);
    URHO3D_ATTRIBUTE_EX("Collision Mask", int, collisionMask_, MarkBodyDirty, DEFAULT_COLLISION_MASK_2, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Contact Threshold", GetContactProcessingThreshold, SetContactProcessingThreshold, float, BT_LARGE_FLOAT,
        AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("CCD Radius", GetCcdRadius, SetCcdRadius, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("CCD Motion Threshold", GetCcdMotionThreshold, SetCcdMotionThreshold, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Network Angular Velocity", GetNetAngularVelocityAttr, SetNetAngularVelocityAttr, PODVector<unsigned char>,
        Variant::emptyBuffer, AM_NET | AM_LATESTDATA | AM_NOEDIT);
    URHO3D_ENUM_ATTRIBUTE_EX("Collision Event Mode", collisionEventMode_, MarkBodyDirty, collisionEventModeNames2, COLLISION_ACTIVE_2, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Use Gravity", GetUseGravity, SetUseGravity, bool, true, AM_DEFAULT);
    URHO3D_ATTRIBUTE_EX("Is Kinematic", bool, kinematic_, MarkBodyDirty, false, AM_DEFAULT);
    URHO3D_ATTRIBUTE_EX("Is Trigger", bool, trigger_, MarkBodyDirty, false, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Gravity Override", GetGravityOverride, SetGravityOverride, Vector3, Vector3::ZERO, AM_DEFAULT);
}

void RigidBody2::ApplyAttributes()
{
    if (readdBody_)
        AddBodyToWorld();
}

void RigidBody2::OnSetEnabled()
{
    bool enabled = IsEnabledEffective();

    if (enabled && !inWorld_)
        AddBodyToWorld();
    else if (!enabled && inWorld_)
        RemoveBodyFromWorld();
}

void RigidBody2::getWorldTransform(btTransform& worldTrans) const
{
    // We may be in a pathological state where a RigidBody exists without a scene node when this callback is fired,
    // so check to be sure
    if (node_)
    {
        lastPosition_ = node_->GetPosition();
        lastRotation_ = node_->GetRotation();
        worldTrans.setOrigin(ToBtVector3(lastPosition_ + lastRotation_ * centerOfMass_));
        worldTrans.setRotation(ToBtQuaternion(lastRotation_));
    }

    hasSimulated_ = true;
}

void RigidBody2::setWorldTransform(const btTransform& worldTrans)
{
    Quaternion newWorldRotation = ToQuaternion(worldTrans.getRotation());
    Vector3 newWorldPosition = ToVector3(worldTrans.getOrigin()) - newWorldRotation * centerOfMass_;
    RigidBody2* parentRigidBody = nullptr;

    // It is possible that the RigidBody component has been kept alive via a shared pointer,
    // while its scene node has already been destroyed
    if (node_)
    {
        // If the rigid body is parented to another rigid body, can not set the transform immediately.
        // In that case store it to PhysicsWorld for delayed assignment
        Node* parent = node_->GetParent();
        if (parent != GetScene() && parent)
            parentRigidBody = parent->GetComponent<RigidBody2>();

        if (!parentRigidBody)
            ApplyWorldTransform(newWorldPosition, newWorldRotation);
        else
        {
            DelayedWorldTransform2 delayed;
            delayed.rigidBody_ = this;
            delayed.parentRigidBody_ = parentRigidBody;
            delayed.worldPosition_ = newWorldPosition;
            delayed.worldRotation_ = newWorldRotation;
            physicsWorld_->AddDelayedWorldTransform(delayed);
        }

        MarkNetworkUpdate();
    }

    hasSimulated_ = true;
}

void RigidBody2::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
    if (debug && physicsWorld_ && body_ && IsEnabledEffective())
    {
        physicsWorld_->SetDebugRenderer(debug);
        physicsWorld_->SetDebugDepthTest(depthTest);

        btDiscreteDynamicsWorld* world = physicsWorld_->GetWorld();
        world->debugDrawObject(body_->getWorldTransform(), shiftedCompoundShape_.Get(), IsActive() ? btVector3(1.0f, 1.0f, 1.0f) :
            btVector3(0.0f, 1.0f, 0.0f));

        physicsWorld_->SetDebugRenderer(nullptr);
    }
}

void RigidBody2::SetMass(float mass)
{
    mass = Max(mass, 0.0f);

    if (mass != mass_)
    {
        mass_ = mass;
        AddBodyToWorld();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetPosition(const Vector3& position)
{
    if (body_)
    {
        btTransform& worldTrans = body_->getWorldTransform();
        worldTrans.setOrigin(ToBtVector3(position + ToQuaternion(worldTrans.getRotation()) * centerOfMass_));

        // When forcing the physics position, set also interpolated position so that there is no jitter
        // When not inside the simulation loop, this may lead to erratic movement of parented rigidbodies
        // so skip in that case. Exception made before first simulation tick so that interpolation position
        // of e.g. instantiated prefabs will be correct from the start
        if (!hasSimulated_ || physicsWorld_->IsSimulating())
        {
            btTransform interpTrans = body_->getInterpolationWorldTransform();
            interpTrans.setOrigin(worldTrans.getOrigin());
            body_->setInterpolationWorldTransform(interpTrans);
        }

        Activate();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetRotation(const Quaternion& rotation)
{
    if (body_)
    {
        Vector3 oldPosition = GetPosition();
        btTransform& worldTrans = body_->getWorldTransform();
        worldTrans.setRotation(ToBtQuaternion(rotation));
        if (!centerOfMass_.Equals(Vector3::ZERO))
            worldTrans.setOrigin(ToBtVector3(oldPosition + rotation * centerOfMass_));

        if (!hasSimulated_ || physicsWorld_->IsSimulating())
        {
            btTransform interpTrans = body_->getInterpolationWorldTransform();
            interpTrans.setRotation(worldTrans.getRotation());
            if (!centerOfMass_.Equals(Vector3::ZERO))
                interpTrans.setOrigin(worldTrans.getOrigin());
            body_->setInterpolationWorldTransform(interpTrans);
        }

        body_->updateInertiaTensor();

        Activate();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetTransform(const Vector3& position, const Quaternion& rotation)
{
    if (body_)
    {
        btTransform& worldTrans = body_->getWorldTransform();
        worldTrans.setRotation(ToBtQuaternion(rotation));
        worldTrans.setOrigin(ToBtVector3(position + rotation * centerOfMass_));

        if (!hasSimulated_ || physicsWorld_->IsSimulating())
        {
            btTransform interpTrans = body_->getInterpolationWorldTransform();
            interpTrans.setOrigin(worldTrans.getOrigin());
            interpTrans.setRotation(worldTrans.getRotation());
            body_->setInterpolationWorldTransform(interpTrans);
        }

        body_->updateInertiaTensor();

        Activate();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetLinearVelocity(const Vector3& velocity)
{
    if (body_)
    {
        body_->setLinearVelocity(ToBtVector3(velocity));
        if (velocity != Vector3::ZERO)
            Activate();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetLinearFactor(const Vector3& factor)
{
    if (body_)
    {
        body_->setLinearFactor(ToBtVector3(factor));
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetLinearRestThreshold(float threshold)
{
    if (body_)
    {
        body_->setSleepingThresholds(threshold, body_->getAngularSleepingThreshold());
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetLinearDamping(float damping)
{
    if (body_)
    {
        body_->setDamping(damping, body_->getAngularDamping());
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetAngularVelocity(const Vector3& velocity)
{
    if (body_)
    {
        body_->setAngularVelocity(ToBtVector3(velocity));
        if (velocity != Vector3::ZERO)
            Activate();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetAngularFactor(const Vector3& factor)
{
    if (body_)
    {
        body_->setAngularFactor(ToBtVector3(factor));
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetAngularRestThreshold(float threshold)
{
    if (body_)
    {
        body_->setSleepingThresholds(body_->getLinearSleepingThreshold(), threshold);
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetAngularDamping(float damping)
{
    if (body_)
    {
        body_->setDamping(body_->getLinearDamping(), damping);
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetFriction(float friction)
{
    if (body_)
    {
        body_->setFriction(friction);
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetAnisotropicFriction(const Vector3& friction)
{
    if (body_)
    {
        body_->setAnisotropicFriction(ToBtVector3(friction));
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetRollingFriction(float friction)
{
    if (body_)
    {
        body_->setRollingFriction(friction);
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetRestitution(float restitution)
{
    if (body_)
    {
        body_->setRestitution(restitution);
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetContactProcessingThreshold(float threshold)
{
    if (body_)
    {
        body_->setContactProcessingThreshold(threshold);
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetCcdRadius(float radius)
{
    radius = Max(radius, 0.0f);
    if (body_)
    {
        body_->setCcdSweptSphereRadius(radius);
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetCcdMotionThreshold(float threshold)
{
    threshold = Max(threshold, 0.0f);
    if (body_)
    {
        body_->setCcdMotionThreshold(threshold);
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetUseGravity(bool enable)
{
    if (enable != useGravity_)
    {
        useGravity_ = enable;
        UpdateGravity();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetGravityOverride(const Vector3& gravity)
{
    if (gravity != gravityOverride_)
    {
        gravityOverride_ = gravity;
        UpdateGravity();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetKinematic(bool enable)
{
    if (enable != kinematic_)
    {
        kinematic_ = enable;
        AddBodyToWorld();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetTrigger(bool enable)
{
    if (enable != trigger_)
    {
        trigger_ = enable;
        AddBodyToWorld();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetCollisionLayer(unsigned layer)
{
    if (layer != collisionLayer_)
    {
        collisionLayer_ = layer;
        AddBodyToWorld();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetCollisionMask(unsigned mask)
{
    if (mask != collisionMask_)
    {
        collisionMask_ = mask;
        AddBodyToWorld();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetCollisionLayerAndMask(unsigned layer, unsigned mask)
{
    if (layer != collisionLayer_ || mask != collisionMask_)
    {
        collisionLayer_ = layer;
        collisionMask_ = mask;
        AddBodyToWorld();
        MarkNetworkUpdate();
    }
}

void RigidBody2::SetCollisionEventMode(CollisionEventMode2 mode)
{
    collisionEventMode_ = mode;
    MarkNetworkUpdate();
}

void RigidBody2::ApplyForce(const Vector3& force)
{
    if (body_ && force != Vector3::ZERO)
    {
        Activate();
        body_->applyCentralForce(ToBtVector3(force));
    }
}

void RigidBody2::ApplyForce(const Vector3& force, const Vector3& position)
{
    if (body_ && force != Vector3::ZERO)
    {
        Activate();
        body_->applyForce(ToBtVector3(force), ToBtVector3(position - centerOfMass_));
    }
}

void RigidBody2::ApplyTorque(const Vector3& torque)
{
    if (body_ && torque != Vector3::ZERO)
    {
        Activate();
        body_->applyTorque(ToBtVector3(torque));
    }
}

void RigidBody2::ApplyImpulse(const Vector3& impulse)
{
    if (body_ && impulse != Vector3::ZERO)
    {
        Activate();
        body_->applyCentralImpulse(ToBtVector3(impulse));
    }
}

void RigidBody2::ApplyImpulse(const Vector3& impulse, const Vector3& position)
{
    if (body_ && impulse != Vector3::ZERO)
    {
        Activate();
        body_->applyImpulse(ToBtVector3(impulse), ToBtVector3(position - centerOfMass_));
    }
}

void RigidBody2::ApplyTorqueImpulse(const Vector3& torque)
{
    if (body_ && torque != Vector3::ZERO)
    {
        Activate();
        body_->applyTorqueImpulse(ToBtVector3(torque));
    }
}

void RigidBody2::ResetForces()
{
    if (body_)
        body_->clearForces();
}

void RigidBody2::Activate()
{
    if (body_ && mass_ > 0.0f)
        body_->activate(true);
}

void RigidBody2::ReAddBodyToWorld()
{
    if (body_ && inWorld_)
        AddBodyToWorld();
}

void RigidBody2::DisableMassUpdate()
{
    enableMassUpdate_ = false;
}

void RigidBody2::EnableMassUpdate()
{
    if (!enableMassUpdate_)
    {
        enableMassUpdate_ = true;
        UpdateMass();
    }
}

Vector3 RigidBody2::GetPosition() const
{
    if (body_)
    {
        const btTransform& transform = body_->getWorldTransform();
        return ToVector3(transform.getOrigin()) - ToQuaternion(transform.getRotation()) * centerOfMass_;
    }
    else
        return Vector3::ZERO;
}

Vector3d RigidBody2::GetPositiond() const
{
    if ( body_ )
    {
        const btTransform & transform = body_->getWorldTransform();
        return ToVector3d(transform.getOrigin()) - ToQuaternion(transform.getRotation()) * centerOfMass_;
    }
    else
        return Vector3d::ZERO;
}

Quaternion RigidBody2::GetRotation() const
{
    return body_ ? ToQuaternion(body_->getWorldTransform().getRotation()) : Quaternion::IDENTITY;
}

Quaterniond RigidBody2::GetRotationd() const
{
    return body_ ? ToQuaterniond(body_->getWorldTransform().getRotation()) : Quaterniond::IDENTITY;
}

Vector3 RigidBody2::GetLinearVelocity() const
{
    return body_ ? ToVector3(body_->getLinearVelocity()) : Vector3::ZERO;
}

Vector3d RigidBody2::GetLinearVelocityd() const
{
    return body_ ? ToVector3d(body_->getLinearVelocity()) : Vector3::ZERO;
}

Vector3 RigidBody2::GetLinearFactor() const
{
    return body_ ? ToVector3(body_->getLinearFactor()) : Vector3::ZERO;
}

Vector3 RigidBody2::GetVelocityAtPoint(const Vector3& position) const
{
    return body_ ? ToVector3(body_->getVelocityInLocalPoint(ToBtVector3(position - centerOfMass_))) : Vector3::ZERO;
}

float RigidBody2::GetLinearRestThreshold() const
{
    return body_ ? body_->getLinearSleepingThreshold() : 0.0f;
}

float RigidBody2::GetLinearDamping() const
{
    return body_ ? body_->getLinearDamping() : 0.0f;
}

Vector3 RigidBody2::GetAngularVelocity() const
{
    return body_ ? ToVector3(body_->getAngularVelocity()) : Vector3::ZERO;
}

Vector3d RigidBody2::GetAngularVelocityd() const
{
    return body_ ? ToVector3d(body_->getAngularVelocity()) : Vector3d::ZERO;
}

Vector3 RigidBody2::GetAngularFactor() const
{
    return body_ ? ToVector3(body_->getAngularFactor()) : Vector3::ZERO;
}

float RigidBody2::GetAngularRestThreshold() const
{
    return body_ ? body_->getAngularSleepingThreshold() : 0.0f;
}

float RigidBody2::GetAngularDamping() const
{
    return body_ ? body_->getAngularDamping() : 0.0f;
}

float RigidBody2::GetFriction() const
{
    return body_ ? body_->getFriction() : 0.0f;
}

Vector3 RigidBody2::GetAnisotropicFriction() const
{
    return body_ ? ToVector3(body_->getAnisotropicFriction()) : Vector3::ZERO;
}

float RigidBody2::GetRollingFriction() const
{
    return body_ ? body_->getRollingFriction() : 0.0f;
}

float RigidBody2::GetRestitution() const
{
    return body_ ? body_->getRestitution() : 0.0f;
}

float RigidBody2::GetContactProcessingThreshold() const
{
    return body_ ? body_->getContactProcessingThreshold() : 0.0f;
}

float RigidBody2::GetCcdRadius() const
{
    return body_ ? body_->getCcdSweptSphereRadius() : 0.0f;
}

float RigidBody2::GetCcdMotionThreshold() const
{
    return body_ ? body_->getCcdMotionThreshold() : 0.0f;
}

bool RigidBody2::IsActive() const
{
    return body_ ? body_->isActive() : false;
}

void RigidBody2::GetCollidingBodies(PODVector<RigidBody2*>& result) const
{
    if (physicsWorld_)
        physicsWorld_->GetCollidingBodies(result, this);
    else
        result.Clear();
}

void RigidBody2::ApplyWorldTransform(const Vector3& newWorldPosition, const Quaternion& newWorldRotation)
{
    // In case of holding an extra reference to the RigidBody, this could be called in a situation
    // where node is already null
    if (!node_ || !physicsWorld_)
        return;

    physicsWorld_->SetApplyingTransforms(true);

    // Apply transform to the SmoothedTransform component instead of node transform if available
    if (smoothedTransform_)
    {
        smoothedTransform_->SetTargetPosition(newWorldPosition);
        smoothedTransform_->SetTargetRotation(newWorldRotation);
        lastPosition_ = newWorldPosition;
        lastRotation_ = newWorldRotation;
    }
    else
    {
        node_->SetPosition(newWorldPosition);
        node_->SetRotation(newWorldRotation);
        lastPosition_ = node_->GetPosition();
        lastRotation_ = node_->GetRotation();
    }

    physicsWorld_->SetApplyingTransforms(false);
}

void RigidBody2::UpdateMass()
{
    if (!body_ || !enableMassUpdate_)
        return;

    btTransform principal;
    principal.setRotation(btQuaternion::getIdentity());
    principal.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

    // Calculate center of mass shift from all the collision shapes
    auto numShapes = (unsigned)compoundShape_->getNumChildShapes();
    if (numShapes)
    {
        PODVector<btScalar> masses(numShapes);
        for (unsigned i = 0; i < numShapes; ++i)
        {
            // The actual mass does not matter, divide evenly between child shapes
            masses[i] = 1.0f;
        }

        btVector3 inertia(0.0f, 0.0f, 0.0f);
        compoundShape_->calculatePrincipalAxisTransform(&masses[0], principal, inertia);
    }

    // Add child shapes to shifted compound shape with adjusted offset
    while (shiftedCompoundShape_->getNumChildShapes())
        shiftedCompoundShape_->removeChildShapeByIndex(shiftedCompoundShape_->getNumChildShapes() - 1);
    for (unsigned i = 0; i < numShapes; ++i)
    {
        btTransform adjusted = compoundShape_->getChildTransform(i);
        adjusted.setOrigin(adjusted.getOrigin() - principal.getOrigin());
        shiftedCompoundShape_->addChildShape(adjusted, compoundShape_->getChildShape(i));
    }

    // If shifted compound shape has only one child with no offset/rotation, use the child shape
    // directly as the rigid body collision shape for better collision detection performance
    bool useCompound = !numShapes || numShapes > 1;
    if (!useCompound)
    {
        const btTransform& childTransform = shiftedCompoundShape_->getChildTransform(0);
        if (!ToVector3(childTransform.getOrigin()).Equals(Vector3::ZERO) ||
            !ToQuaternion(childTransform.getRotation()).Equals(Quaternion::IDENTITY))
            useCompound = true;
    }

    btCollisionShape* oldCollisionShape = body_->getCollisionShape();
    body_->setCollisionShape(useCompound ? shiftedCompoundShape_.Get() : shiftedCompoundShape_->getChildShape(0));

    // If we have one shape and this is a triangle mesh, we use a custom material callback in order to adjust internal edges
    if (!useCompound && body_->getCollisionShape()->getShapeType() == SCALED_TRIANGLE_MESH_SHAPE_PROXYTYPE &&
        physicsWorld_->GetInternalEdge())
        body_->setCollisionFlags(body_->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    else
        body_->setCollisionFlags(body_->getCollisionFlags() & ~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    // Reapply rigid body position with new center of mass shift
    Vector3 oldPosition = GetPosition();
    centerOfMass_ = ToVector3(principal.getOrigin());
    SetPosition(oldPosition);

    // Calculate final inertia
    btVector3 localInertia(0.0f, 0.0f, 0.0f);
    if (mass_ > 0.0f)
        shiftedCompoundShape_->calculateLocalInertia(mass_, localInertia);
    body_->setMassProps(mass_, localInertia);
    body_->updateInertiaTensor();

    // Reapply constraint positions for new center of mass shift
    if (node_)
    {
        for (PODVector<Constraint2*>::Iterator i = constraints_.Begin(); i != constraints_.End(); ++i)
            (*i)->ApplyFrames();
    }

    // Readd body to world to reset Bullet collision cache if collision shape was changed (issue #2064)
    if (inWorld_ && body_->getCollisionShape() != oldCollisionShape && physicsWorld_)
    {
        btDiscreteDynamicsWorld* world = physicsWorld_->GetWorld();
        world->removeRigidBody(body_.Get());
        world->addRigidBody(body_.Get(), (short)collisionLayer_, (short)collisionMask_);
    }
}

void RigidBody2::UpdateGravity()
{
    if (physicsWorld_ && body_)
    {
        btDiscreteDynamicsWorld* world = physicsWorld_->GetWorld();

        int flags = body_->getFlags();
        if (useGravity_ && gravityOverride_ == Vector3::ZERO)
            flags &= ~BT_DISABLE_WORLD_GRAVITY;
        else
            flags |= BT_DISABLE_WORLD_GRAVITY;
        body_->setFlags(flags);

        if (useGravity_)
        {
            // If override vector is zero, use world's gravity
            if (gravityOverride_ == Vector3::ZERO)
                body_->setGravity(world->getGravity());
            else
                body_->setGravity(ToBtVector3(gravityOverride_));
        }
        else
            body_->setGravity(btVector3(0.0f, 0.0f, 0.0f));
    }
}

void RigidBody2::SetNetAngularVelocityAttr(const PODVector<unsigned char>& value)
{
    float maxVelocity = physicsWorld_ ? physicsWorld_->GetMaxNetworkAngularVelocity() : DEFAULT_MAX_NETWORK_ANGULAR_VELOCITY_2;
    MemoryBuffer buf(value);
    SetAngularVelocity(buf.ReadPackedVector3(maxVelocity));
}

const PODVector<unsigned char>& RigidBody2::GetNetAngularVelocityAttr() const
{
    float maxVelocity = physicsWorld_ ? physicsWorld_->GetMaxNetworkAngularVelocity() : DEFAULT_MAX_NETWORK_ANGULAR_VELOCITY_2;
    attrBuffer_.Clear();
    attrBuffer_.WritePackedVector3(GetAngularVelocity(), maxVelocity);
    return attrBuffer_.GetBuffer();
}

void RigidBody2::AddConstraint(Constraint2* constraint)
{
    constraints_.Push(constraint);
}

void RigidBody2::RemoveConstraint(Constraint2* constraint)
{
    constraints_.Remove(constraint);
    // A constraint being removed should possibly cause the object to eg. start falling, so activate
    Activate();
}

void RigidBody2::ReleaseBody()
{
    if (body_)
    {
        // Release all constraints which refer to this body
        // Make a copy for iteration
        PODVector<Constraint2*> constraints = constraints_;
        for (PODVector<Constraint2*>::Iterator i = constraints.Begin(); i != constraints.End(); ++i)
            (*i)->ReleaseConstraint();

        RemoveBodyFromWorld();

        body_.Reset();
    }
}

void RigidBody2::OnMarkedDirty(Node* node)
{
    // If node transform changes, apply it back to the physics transform. However, do not do this when a SmoothedTransform
    // is in use, because in that case the node transform will be constantly updated into smoothed, possibly non-physical
    // states; rather follow the SmoothedTransform target transform directly
    // Also, for kinematic objects Bullet asks the position from us, so we do not need to apply ourselves
    // (exception: initial setting of transform)
    if ((!kinematic_ || !hasSimulated_) && (!physicsWorld_ || !physicsWorld_->IsApplyingTransforms()) && !smoothedTransform_)
    {
        // Physics operations are not safe from worker threads
        Scene* scene = GetScene();
        if (scene && scene->IsThreadedUpdate())
        {
            scene->DelayedMarkedDirty(this);
            return;
        }

        // Check if transform has changed from the last one set in ApplyWorldTransform()
        Vector3 newPosition = node_->GetPosition();
        Quaternion newRotation = node_->GetRotation();

        if (!newRotation.Equals(lastRotation_))
        {
            lastRotation_ = newRotation;
            SetRotation(newRotation);
        }
        if (!newPosition.Equals(lastPosition_))
        {
            lastPosition_ = newPosition;
            SetPosition(newPosition);
        }
    }
}

void RigidBody2::OnNodeSet(Node* node)
{
    if (node)
    {
        node->AddListener(this);

        // Listen to parent changes and add to world.
        subscribeToParentChanges();
    }
}

void RigidBody2::OnSceneSet(Scene* scene)
{
    /*
    if (scene)
    {
        if (scene == node_)
            URHO3D_LOGWARNING(GetTypeName() + " should not be created to the root scene node");

        physicsWorld_ = scene->GetOrCreateComponent<PhysicsWorld2>();
        physicsWorld_->AddRigidBody(this);

        AddBodyToWorld();
    }
    else
    {
        ReleaseBody();

        if (physicsWorld_)
            physicsWorld_->RemoveRigidBody(this);
    }
    */
}

void RigidBody2::AddBodyToWorld()
{
    if (!physicsWorld_)
        return;

    URHO3D_PROFILE(AddBodyToWorld);

    if (mass_ < 0.0f)
        mass_ = 0.0f;

    if (body_)
        RemoveBodyFromWorld();
    else
    {
        // Correct inertia will be calculated below
        btVector3 localInertia(0.0f, 0.0f, 0.0f);
        body_ = new btRigidBody(mass_, this, shiftedCompoundShape_.Get(), localInertia);
        body_->setUserPointer(this);

        // Check for existence of the SmoothedTransform component, which should be created by now in network client mode.
        // If it exists, subscribe to its change events
        smoothedTransform_ = GetComponent<SmoothedTransform>();
        if (smoothedTransform_)
        {
            SubscribeToEvent(smoothedTransform_, E_TARGETPOSITION, URHO3D_HANDLER(RigidBody2, HandleTargetPosition));
            SubscribeToEvent(smoothedTransform_, E_TARGETROTATION, URHO3D_HANDLER(RigidBody2, HandleTargetRotation));
        }

        // Check if CollisionShapes already exist in the node and add them to the compound shape.
        // Do not update mass yet, but do it once all shapes have been added
        PODVector<CollisionShape2*> shapes;
        node_->GetComponents<CollisionShape2>(shapes);
        for (PODVector<CollisionShape2*>::Iterator i = shapes.Begin(); i != shapes.End(); ++i)
            (*i)->NotifyRigidBody(false);

        // Check if this node contains Constraint components that were waiting for the rigid body to be created, and signal them
        // to create themselves now
        PODVector<Constraint2*> constraints;
        node_->GetComponents<Constraint2>(constraints);
        for (PODVector<Constraint2*>::Iterator i = constraints.Begin(); i != constraints.End(); ++i)
            (*i)->CreateConstraint();
    }

    UpdateMass();
    UpdateGravity();

    int flags = body_->getCollisionFlags();
    if (trigger_)
        flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
    else
        flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;
    if (kinematic_)
        flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
    else
        flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
    body_->setCollisionFlags(flags);
    body_->forceActivationState(kinematic_ ? DISABLE_DEACTIVATION : ISLAND_SLEEPING);

    if (!IsEnabledEffective())
        return;

    btDiscreteDynamicsWorld* world = physicsWorld_->GetWorld();
    world->addRigidBody(body_.Get(), (short)collisionLayer_, (short)collisionMask_);
    inWorld_ = true;
    readdBody_ = false;
    hasSimulated_ = false;

    if (mass_ > 0.0f)
        Activate();
    else
    {
        SetLinearVelocity(Vector3::ZERO);
        SetAngularVelocity(Vector3::ZERO);
    }
}

void RigidBody2::RemoveBodyFromWorld()
{
    if (physicsWorld_ && body_ && inWorld_)
    {
        btDiscreteDynamicsWorld* world = physicsWorld_->GetWorld();
        world->removeRigidBody(body_.Get());
        inWorld_ = false;
    }
}

void RigidBody2::HandleTargetPosition(StringHash eventType, VariantMap& eventData)
{
    // Copy the smoothing target position to the rigid body
    if (!physicsWorld_ || !physicsWorld_->IsApplyingTransforms())
        SetPosition(static_cast<SmoothedTransform*>(GetEventSender())->GetTargetPosition());
}

void RigidBody2::HandleTargetRotation(StringHash eventType, VariantMap& eventData)
{
    // Copy the smoothing target rotation to the rigid body
    if (!physicsWorld_ || !physicsWorld_->IsApplyingTransforms())
        SetRotation(static_cast<SmoothedTransform*>(GetEventSender())->GetTargetRotation());
}

void RigidBody2::subscribeToParentChanges()
{
    SubscribeToEvent( E_NODEREMOVED, URHO3D_HANDLER( RigidBody2, OnNodeRemoved ) );
    SubscribeToEvent( E_NODEADDED,   URHO3D_HANDLER( RigidBody2, OnNodeAdded ) );

    addToWorld();
}

void RigidBody2::OnNodeRemoved( StringHash eventType, VariantMap & eventData )
{
    Variant & n = eventData[NodeRemoved::P_NODE];
    Node * self = static_cast<Node *>( n.GetPtr() );
    Node * node = GetNode();
    if ( self != node )
        return;
    Variant & v = eventData[NodeRemoved::P_PARENT] ;
    Node * msgParent = static_cast<Node *>( v.GetPtr() );
    Node * parent = GetNode()->GetParent();
    if ( parent != msgParent )
        return;
    removeFromWorld();
}

void RigidBody2::OnNodeAdded( StringHash eventType, VariantMap & eventData )
{
    Variant & n = eventData[NodeAdded::P_NODE];
    Node * self = static_cast<Node *>( n.GetPtr() );
    Node * node = GetNode();
    if ( self != node )
        return;
    Variant & v = eventData[NodeAdded::P_PARENT] ;
    Node * msgParent = static_cast<Node *>( v.GetPtr() );
    Node * parent = GetNode()->GetParent();
    if ( parent != msgParent )
        return;
    addToWorld();
}

void RigidBody2::removeFromWorld()
{
    ReleaseBody();

    if (physicsWorld_)
        physicsWorld_->RemoveRigidBody(this);
}

void RigidBody2::addToWorld()
{
    Node * node = GetNode();
    PhysicsWorld2 * newWorld = PhysicsWorld2::getWorld( node );
    if ( !newWorld )
        return;

    physicsWorld_ = WeakPtr<PhysicsWorld2>( newWorld );
    physicsWorld_->AddRigidBody(this);

    AddBodyToWorld();
}

}
