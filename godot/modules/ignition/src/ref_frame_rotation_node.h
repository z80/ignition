
#ifndef __REF_FRAME_ROTATION_NODE_H_
#define __REF_FRAME_ROTATION_NODE_H_

#include "ref_frame_node.h"
#include "celestial_rotation.h"


namespace Ign
{

class RefFrameRotationNode: public RefFrameNode
{
	GDCLASS(RefFrameRotationNode, RefFrameNode);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameRotationNode();
	virtual ~RefFrameRotationNode();

	virtual void set_se3_raw( const SE3 & se3 ) override;

	void init( const Vector3 & up, real_t period_hrs );
	void process_rf( real_t dt, Node * rf );

	virtual Dictionary serialize() override;
	virtual bool deserialize( const Dictionary & data ) override;

	// Compute forces, integrate dynamics.
	virtual void _ign_physics_pre_process( real_t delta ) override;
	// Set positions, place visuals.
	virtual void _ign_physics_process( real_t delta ) override;
	// Place camera.
	virtual void _ign_physics_post_process( real_t delta ) override;

	//void dbg_process( real_t delta );
	//void dbg_reset();
	//void dbg_print();
public:
	CelestialRotation cr;

	//Integer qty_calls;
	//Float total_time;
};



}




#endif




