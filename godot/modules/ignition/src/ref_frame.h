
#ifndef __REF_FRAME_H_
#define __REF_FRAME_H_

#include "scene/3d/spatial.h"
//#include "core/reference.h"
#include "core/math/vector3.h"
#include "core/math/quat.h"
#include "core/math/transform.h"

#include "core/vector.h"

#include "se3.h"

using namespace Ign;

class RefFrameTree;

class RefFrame: public Reference
{
	GDCLASS(RefFrame, Reference);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	RefFrame();
	virtual ~RefFrame();

	void set_name( const String & name );
	const String & name() const;

	void set_r( const Vector3 & r );
	void set_q( const Quat & q );
	void set_v( const Vector3 & v );
	void set_w( const Vector3 & w );

	void set_rf_transform( const Transform & t );
	Transform rf_transform() const;

	Vector3 r() const;
	Quat    q() const;
	Vector3 v() const;
	Vector3 w() const;

	Vector3 r_root() const;
	Quat    q_root() const;
	Vector3 v_root() const;
	Vector3 w_root() const;
	Transform t_root() const;

	void set_origin( Ref<Reference> parent );
	Ref<Reference> origin() const;

	void set_root( Ref<Reference> parent );
	Ref<Reference> root() const;

	bool in_tree() const;

	/// Change origin without changing absolute position in space.
	void change_origin( Ref<Reference> origin );

	/// Compute state relative to the "root_" specified.
	void compute_relative_to_root();

	/// Related to jump.
	void set_jump_r( const Vector3 & r );
	void set_jump_q( const Quat & q );
	void set_jump_v( const Vector3 & v );
	void set_jump_w( const Vector3 & w );
	void set_jump_transform( const Transform & t );

	void set_obj_r( const Vector3 & r );
	void set_obj_q( const Quat & q );
	void set_obj_v( const Vector3 & v );
	void set_obj_w( const Vector3 & w );
	void set_obj_transform( const Transform & t );

	void calc_obj_jump_state();

	Vector3 obj_r() const;
	Quat    obj_q() const;
	Vector3 obj_v() const;
	Vector3 obj_w() const;
	Transform obj_transform() const;

	void apply_jump();


	/// Compute relative state in the most generic way.
	/// Provide two points in local and in root frames.
	/// It comutes "se3_local" relative to "se3_root".
	SE3 relative( int root_ind, const SE3 & se3_local=SE3(), const SE3 & se3_root=SE3() );

	String name_;
	SE3    se3_;
	SE3    se3_rel_to_root_;
	SE3    se3_jump_to_;
	SE3    se3_obj_cur_;
	SE3    se3_obj_after_jump_;

	Ref<RefFrameTree> tree_;
	int index_;
	int origin_;
	int root_;

	Vector<RefFrame * > queueA_, queueB_;
};




#endif


