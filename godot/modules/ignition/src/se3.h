
#ifndef __SE3_H_
#define __SE3_H_

#include "vector3d.h"
#include "quaterniond.h"

#include "core/math/vector3.h"
#include "core/math/quat.h"

namespace Ign
{

class SE3
{
public:
	Quaterniond q_;
	Vector3d    r_;
	Vector3d    v_;
	Vector3d    w_;


	SE3();
	~SE3();

	SE3( const Vector3d & r, const Quaterniond & q, const Vector3d & v, const Vector3d & w );
	SE3( const Vector3 & r, const Quat & q, const Vector3 & v, const Vector3 & w );
	SE3( const SE3 & rhs );
	const SE3 & operator=( const SE3 & rhs );
	SE3 operator*( const SE3 & rhs ) const;
	SE3 operator/( const SE3 & rhs ) const;


	SE3 relative_to( const SE3 & o ) const;
	SE3 absolute_to( const SE3 & o ) const;

	void setR( const Vector3 & r );
	void setQ( const Quat & q );
	void setV( const Vector3 & v );
	void setW( const Vector3 & w );
	Vector3 r() const;
	Quat    q() const;
	Vector3 v() const;
	Vector3 w() const;
};

}


#endif


