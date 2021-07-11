
#ifndef __SAVE_LOAD_H_
#define __SAVE_LOAD_H_

#include "vector3d.h"
#include "quaterniond.h"
#include "matrix3d.h"
#include "core/dictionary.h"
#include "core/variant.h"


namespace Ign
{

void serialize_vector( const Vector3d & v, const String & name, Dictionary & data );
Vector3d deserialize_vector( const String & name, const Dictionary & data );

void serialize_quat( const Quaterniond & v, const String & name, Dictionary & data );
Quaterniond deserialize_quat( const String & name, const Dictionary & data );

void serialize_matrix( const Matrix3d & v, const String & name, Dictionary & data );
Matrix3d deserialize_matrix( const String & name, const Dictionary & data );

}



#endif





