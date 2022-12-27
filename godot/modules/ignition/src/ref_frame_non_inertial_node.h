
#ifndef __REF_FRAME_NON_INERTIAL_NODE_H_
#define __REF_FRAME_NON_INERTIAL_NODE_H_

#include "ref_frame_node.h"
#include "numerical_motion.h"
#include "distance_scaler_base_ref.h"

namespace Ign
{

class RefFrameMotionNode;
class RefFrameRotationNode;
class RefFrameBodyNode;

class RefFrameNonInertialNode: public RefFrameNode
{
	GDCLASS(RefFrameNonInertialNode, RefFrameNode);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameNonInertialNode();
	virtual ~RefFrameNonInertialNode();

	void set_physics_mode( bool en );
	bool get_physics_node() const;

	virtual void _ign_pre_process( real_t delta ) override;
	virtual void _ign_process( real_t delta ) override;
	virtual void _ign_post_process( real_t delta ) override;

	virtual void _ign_physics_pre_process( real_t delta ) override;
	virtual void _ign_physics_process( real_t delta ) override;
	virtual void _ign_physics_post_process( real_t delta ) override;

public:
	bool physics_mode;

	// Parent gravitational bodies. For computing centrifugal acceleration.
	Vector<RefFrameMotionNode *> parent_gms;
	// Rotational ref. frames. This is for computing non-inertial forces caused by rotational motion.
	Vector<RefFrameRotationNode *> parent_rots;
	// All gravitational bodies. For computing gravitational forces.
	Vector<RefFrameMotionNode * > all_gms;
	// All child ref. frames to either apply forces or evolve.
	// If forcess are applied, these are real physicsl bodies.
	// If evolving, it should be an array of super bodies (assemblies).
	Vector<RefFrameBodyNode *> all_bodies;

	Vector3d        centrifugal_acc;
	NumericalMotion nm;
};


}




#endif


