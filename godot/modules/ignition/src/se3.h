
#ifndef __SE3_H_
#define __SE3_H_

#include "vector3d.h"
#include "quaterniond.h"

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

	SE3( const SE3 & rhs );
	const SE3 & operator=( const SE3 & rhs );
	SE3 operator*( const SE3 & rhs ) const;
	SE3 operator/( const SE3 & rhs ) const;


	SE3 relative_to( const SE3 & o ) const;
	SE3 absolute_to( const SE3 & o ) const;
};

}


#endif


