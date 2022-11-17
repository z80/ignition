

#ifndef __REF_FRAME_NODE_H_
#define __REF_FRAME_NODE_H_

#include "scene/main/node.h"

#include "core/math/vector3.h"
#include "core/math/quat.h"
#include "core/math/transform.h"

#include "core/vector.h"

#include "se3.h"
#include "se3_ref.h"

namespace Ign
{

class RefFrameNode: public Node
{
	GDCLASS(RefFrameNode, Node);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrameNode();
	virtual ~RefFrameNode();

	void set_r( const Vector3 & r );
	void set_q( const Quat & q );
	void set_v( const Vector3 & v );
	void set_w( const Vector3 & w );

	void set_t( const Transform & t );
	Transform t() const;

	Vector3 r() const;
	Quat    q() const;
	Vector3 v() const;
	Vector3 w() const;

	Vector3 r_root() const;
	Quat    q_root() const;
	Vector3 v_root() const;
	Vector3 w_root() const;
	Transform t_root() const;

	void set_se3( const Ref<Se3Ref> & se3 );
	Ref<Se3Ref> get_se3() const;

	Ref<Se3Ref> relative_to( Node * origin );
	Ref<Se3Ref> relative_to_se3( Node * origin, const Ref<Se3Ref> & origin_se3 );
	Ref<Se3Ref> se3_relative_to( const Ref<Se3Ref> & object_se3, Node * origin );

	/// Change origin without changing absolute position in space.
	void change_parent( Node * origin );

	/// Compute state relative to the "root_" specified.
	void compute_relative_to_root( Node * root );

	/// Related to jump.
	void set_jump_r( const Vector3 & r );
	void set_jump_q( const Quat & q );
	void set_jump_v( const Vector3 & v );
	void set_jump_w( const Vector3 & w );
	void set_jump_t( const Transform & t );

	void apply_jump();
	void jump_to( Node * dest, const Ref<Se3Ref> & dest_se3 );
	// This is actual implementation.
	void jump_to_( Node * destination, const SE3 & dest_se3 );
	// Callbacks for script notification.
	void _jumped();
	void _parent_jumped();
	void _child_jumped( RefFrameNode * child_ref_frame );
	void _child_left( RefFrameNode * child_ref_frame );

	/// Compute relative state in the most generic way.
	/// Provide two points in local and in root frames.
	/// It comutes "se3_local" relative to "se3_root".
	SE3 relative_( RefFrameNode * root, const SE3 & se3_local=SE3(), const SE3 & se3_root=SE3() );

	RefFrameNode * parent_rf_() const;

	String _unique_name( const String & name_base, Node * parent );


	void set_debug( bool en );
	bool get_debug() const;

	SE3    se3_;
	SE3    se3_root_;
	SE3    se3_jump_to_;

	//SE3    se3_obj_cur_;
	//SE3    se3_obj_rel_to_root_;
	//SE3    se3_obj_after_jump_;

	Vector<RefFrameNode * > queueA_, queueB_;

	bool debug_;
};

}



#endif





