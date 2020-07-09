
#include "gdnative/se3.h"

#include "modules/ignition/src/se3.h"
#include "core/variant.h"

using namespace Ign;

#ifdef __cplusplus
extern "C" {
#endif

	void GDAPI godot_se3_new( godot_se3 *r_dest )
	{

		SE3 * dest = (SE3 *)r_dest;
		*dest = SE3();
	}

	godot_string GDAPI godot_se3_as_string( const godot_se3 *p_self)
	{
		godot_string ret;
		const SE3 *self = (const SE3 *)p_self;
		//memnew_placement( &ret, String(*self) );
		memnew_placement( &ret, String(self->r()) );
		return ret;
	}

	

	godot_se3 GDAPI godot_se3_operator_multiply_se3( const godot_se3 * p_self, const godot_se3 * p_b )
	{
		godot_se3 raw_dest;
		SE3 * dest = (SE3 *)&raw_dest;
		const SE3 * self = (const SE3 *)p_self;
		const SE3 * b = (const SE3 *)p_b;
		*dest = (*self) * (*b);
		return raw_dest;
	}

	godot_se3 GDAPI godot_se3_operator_divide_se3( const godot_se3 * p_self, const godot_se3 * p_b )
	{
		godot_se3 raw_dest;
		SE3 * dest = (SE3 *)&raw_dest;
		const SE3 *self = (const SE3 *)p_self;
		const SE3 *b = (const SE3 *)p_b;
		*dest = *self / *b;
		return raw_dest;
	}

	

	/*void GDAPI godot_vector2_set_x(godot_vector2 *p_self, const godot_real p_x) {
		Vector2 *self = (Vector2 *)p_self;
		self->x = p_x;
	}

	void GDAPI godot_vector2_set_y(godot_vector2 *p_self, const godot_real p_y) {
		Vector2 *self = (Vector2 *)p_self;
		self->y = p_y;
	}

	godot_real GDAPI godot_vector2_get_x(const godot_vector2 *p_self) {
		const Vector2 *self = (const Vector2 *)p_self;
		return self->x;
	}

	godot_real GDAPI godot_vector2_get_y(const godot_vector2 *p_self) {
		const Vector2 *self = (const Vector2 *)p_self;
		return self->y;
	}*/

#ifdef __cplusplus
}
#endif

