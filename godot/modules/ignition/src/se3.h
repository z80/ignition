
#ifndef __SE3_H_
#define __SE3_H_

#include "vector3d.h"
#include "quaterniond.h"

#include "core/math/vector3.h"
#include "core/math/quaternion.h"
#include "core/math/transform_3d.h"

#include "core/variant/dictionary.h"

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
    SE3( const Vector3 & r, const Quaternion & q, const Vector3 & v, const Vector3 & w );
    SE3( const SE3 & rhs );
    const SE3 & operator=( const SE3 & rhs );
    SE3 operator*( const SE3 & rhs ) const;
    SE3 operator/( const SE3 & rhs ) const;
    Vector3d operator*( const Vector3d & r ) const;

    SE3 relative_to( const SE3 & o ) const;
    SE3 absolute_to( const SE3 & o ) const;
    SE3 relative_to_child( const SE3 & ch ) const;
    SE3 inverse() const;

    void set_r( const Vector3 & r );
    void set_q( const Quaternion & q );
    void set_v( const Vector3 & v );
    void set_w( const Vector3 & w );
    void set_transform( const Transform3D & t );
    Vector3 r() const;
    Quaternion q() const;
    Vector3 v() const;
    Vector3 w() const;
    Transform3D transform() const;

    Dictionary serialize() const;
    bool   deserialize( const Dictionary & stri );
};

}


#endif


