
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

class RefFrame: public Spatial
{
	GDCLASS(RefFrame, Spatial);
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

	void set_transform( const Transform & t );
	Transform transform() const;

	Vector3 r() const;
	Quat    q() const;
	Vector3 v() const;
	Vector3 w() const;

	void set_origin( Node * parent );
	Node * origin() const;

	void set_root( Node * parent );
	Node * root() const;

	String name_;
	SE3    se3_;

	RefFrameTree * tree_;
	int index_;
	int origin_;
	int root_;

	Vector<RefFrame * > queueA_, queueB_;
};




#endif


