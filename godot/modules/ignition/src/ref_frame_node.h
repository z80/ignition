

#ifndef __REF_FRAME_NODE_H_
#define __REF_FRAME_NODE_H_

#include "scene/main/node.h"
//#include "core/reference.h"
#include "core/math/vector3.h"
#include "core/math/quat.h"
#include "core/math/transform.h"

#include "core/vector.h"

#include "se3.h"

using namespace Ign;

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

	//void set_obj_r( const Vector3 & r );
	//void set_obj_q( const Quat & q );
	//void set_obj_v( const Vector3 & v );
	//void set_obj_w( const Vector3 & w );
	//void set_obj_t( const Transform & t );

	//void calc_obj_jump_state();

	//Vector3 obj_r() const;
	//Quat    obj_q() const;
	//Vector3 obj_v() const;
	//Vector3 obj_w() const;
	//Transform obj_t() const;

	void apply_jump();

	/// Object transform in local frame and output object relative to root.
	//void calc_obj_relative_to_root( Node * root );
	//Vector3 obj_root_r() const;
	//Quat    obj_root_q() const;
	//Vector3 obj_root_v() const;
	//Vector3 obj_root_w() const;
	//Transform obj_root_t() const;


	/// Compute relative state in the most generic way.
	/// Provide two points in local and in root frames.
	/// It comutes "se3_local" relative to "se3_root".
	SE3 relative_( RefFrameNode * root, const SE3 & se3_local=SE3(), const SE3 & se3_root=SE3() );

	RefFrameNode * parent_rf_() const;

	SE3    se3_;
	SE3    se3_root_;
	SE3    se3_jump_to_;
	//SE3    se3_obj_cur_;
	//SE3    se3_obj_rel_to_root_;
	//SE3    se3_obj_after_jump_;

	Vector<RefFrameNode * > queueA_, queueB_;
};




#endif





