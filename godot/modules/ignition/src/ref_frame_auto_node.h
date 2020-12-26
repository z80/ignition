
#ifndef __REF_FRAME_AUTO_NODE_H_
#define __REF_FRAME_AUTO_NODE_H_

#include "ref_frame_node.h"
#include "core/reference.h"

namespace Ign
{

class SpatialScalerRef;

class RefFrameAutoNode: public RefFrameNode
{
	GDCLASS( RefFrameAutoNode, RefFrameNode );
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();
	void _notification( int p_what );
public:
	RefFrameAutoNode();
	~RefFrameAutoNode();

	void set_distance_scaler( Ref<SpatialScalerRef> new_scaler );
	Ref<SpatialScalerRef> get_distance_scaler() const;

	void set_root_path( const NodePath & root );
	const NodePath & get_root_path() const;

	Transform distance_scaled_t() const;
	real_t    distance_scale() const;

	void set_apply_scale( bool en );
	bool get_apply_scale() const;

private:
	void process_transform();

public:
	NodePath root_path;
	bool apply_scale;
	Vector3d scaled_r;
	Ref<SpatialScalerRef> scaler;
};


}




#endif


