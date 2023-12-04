
#ifndef __REF_FRAME_NON_INERTIAL_NODE_H_
#define __REF_FRAME_NON_INERTIAL_NODE_H_

#include "ref_frame_motion_node.h"
#include "numerical_motion.h"
#include "distance_scaler_base_ref.h"

namespace Ign
{

class RefFrameMotionNode;
class RefFrameRotationNode;
class RefFrameBodyNode;

struct MotionPair
{
	// Motion node orbiting its parent.
	RefFrameMotionNode * node;
	// SE3 of this nore relative to 'this' node.
	SE3                  se3;
};

struct RotationPair
{
	// Rotating ref. frame.
	RefFrameRotationNode * node;
	// SE3 of this node relative to 'this' node.
	SE3                    se3;
};

class RefFrameNonInertialNode: public RefFrameMotionNode
{
	GDCLASS(RefFrameNonInertialNode, RefFrameMotionNode);
	//OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();
	void _notification( int p_notification );

public:
	RefFrameNonInertialNode();
	virtual ~RefFrameNonInertialNode();

	void set_physics_integration( bool en );
	bool get_physics_integration() const;

	void set_physics_mode( bool en );
	bool get_physics_mode() const;

	void set_time_step( real_t dt );
	real_t get_time_step() const;

	virtual Dictionary serialize() override;
	virtual bool deserialize( const Dictionary & data ) override;

	virtual void ign_pre_process( real_t delta ) override;
	virtual void ign_process( real_t delta ) override;
	virtual void ign_post_process( real_t delta ) override;

	virtual void ign_physics_pre_process( real_t delta ) override;
	virtual void ign_physics_process( real_t delta ) override;
	virtual void ign_physics_post_process( real_t delta ) override;

public:
	bool physics_integration;
	bool physics_mode;

	// Parent gravitational bodies. For computing centrifugal acceleration.
	Vector<MotionPair> parent_gms;
	// Rotational ref. frames. This is for computing non-inertial forces caused by rotational motion.
	Vector<RotationPair> parent_rots;
	// All gravitational bodies. For computing gravitational forces.
	Vector<MotionPair> all_gms;
	// All child ref. frames to either apply forces or evolve.
	// If forcess are applied, these are real physicsl bodies.
	// If evolving, it should be an array of super bodies (assemblies).
	Vector<RefFrameBodyNode *> all_bodies;

	NumericalMotion nm;

	List<Node *> nodes;
	void _refresh_force_source_nodes();
	void _refresh_body_nodes();
	void _refresh_super_body_nodes();

	void _compute_relative_se3s();
	Vector3d _compute_combined_acc();
	void _compute_all_accelerations( const Vector3d & combined_orbital_acc );
	void _compute_one_accelearation(  const Vector3d & combined_orbital_acc, RefFrameBodyNode * body );
	void _integrate_super_bodies( Float delta );
};


}




#endif


