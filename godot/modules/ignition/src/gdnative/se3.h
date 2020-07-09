

#ifndef GODOT_SE3_H
#define GODOT_SE3_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GODOT_SE3_SIZE 104

#ifndef GODOT_CORE_API_GODOT_SE3_TYPE_DEFINED
#define GODOT_CORE_API_GODOT_SE3_TYPE_DEFINED
	typedef struct {
		uint8_t _dont_touch_that[GODOT_SE3_SIZE];
	} godot_se3;
#endif

	// reduce extern "C" nesting for VS2013
#ifdef __cplusplus
}
#endif

#include "gdnative/gdnative.h"
//#include "modules/gdnative/include/gdnative/gdnative.h"

#ifdef __cplusplus
extern "C" {
#endif

	void GDAPI godot_se3_new( godot_se3 * r_dest );

	godot_string GDAPI godot_se3_as_string( const godot_se3 * p_self );

	godot_se3 GDAPI godot_se3_operator_multiply_se3( const godot_se3 * p_self, const godot_se3 * p_b );

	godot_se3 GDAPI godot_se3_operator_divide_se3( const godot_se3 * p_self, const godot_se3 * p_b );

	//void GDAPI godot_vector2_set_x(godot_vector2 *p_self, const godot_real p_x);

	//void GDAPI godot_vector2_set_y(godot_vector2 *p_self, const godot_real p_y);

	//godot_real GDAPI godot_vector2_get_x(const godot_vector2 *p_self);

	//godot_real GDAPI godot_vector2_get_y(const godot_vector2 *p_self);

#ifdef __cplusplus
}
#endif

#endif // GODOT_VECTOR2_H



