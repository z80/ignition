
#ifndef __SE3_REF_H_
#define __SE3_REF_H_

#include "se3.h"
#include "core/reference.h"

namespace Ign
{

class Se3Ref: public Reference
{
    GDCLASS(Se3Ref, Reference);
protected:
    static void _bind_methods();

public:
    Se3Ref();
    ~Se3Ref();

    void set_r( const Vector3 & r );
    Vector3 get_r() const;

    void set_v( const Vector3 & v );
    Vector3 get_v() const;

    void set_w( const Vector3 & w );
    Vector3 get_w() const;

    void set_q( const Quat & q );
    Quat get_q() const;

    void set_transform( const Transform & t );
    Transform get_transform() const;

    void copy_from( const Ref<Se3Ref> & ref );
    void copy_r_from( const Ref<Se3Ref> & ref );
    void copy_v_from( const Ref<Se3Ref> & ref );
    void copy_w_from( const Ref<Se3Ref> & ref );
    void copy_q_from( const Ref<Se3Ref> & ref );

    Ref<Se3Ref> mul( const Ref<Se3Ref> & rhs );
    Ref<Se3Ref> div( const Ref<Se3Ref> & rhs );
    Ref<Se3Ref> inverse() const;

    Dictionary serialize() const;
    bool deserialize( const Dictionary & data );

public:
    SE3 se3;
};

}


#endif



