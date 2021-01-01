
#include "height_source_gd.h"
#include "core/print_string.h"
#include "core/script_language.h"

namespace Ign
{

HeightSourceGd::HeightSourceGd()
	: HeightSource()
{
}

HeightSourceGd::~HeightSourceGd()
{
}

bool HeightSourceGd::has_script() const
{
	if ( object.ptr() == nullptr )
		return false;
	const bool has = (object.ptr()->get_script_instance() != nullptr);
	return has;
}

Float HeightSourceGd::height( const Vector3d & at ) const
{
	if ( !has_script() )
	{
		print_error( "HeightSourceGd error: no script assigned" );
		return 0.0;
	}
	ScriptInstance * si = object.ptr()->get_script_instance();
	Variant v_at = Vector3( at.x_, at.y_, at.z_ );
	const Variant *ptr[1] = { &v_at };
	Variant::CallError ce;
	const Variant v_ret = si->call( "height", ptr, 1, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
	{
		print_error( "HeightSourceGd error: expected Vector3 as an argument" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}

Color HeightSourceGd::color( const Vector3d & at, const Vector3d & norm, Float height ) const
{
	if ( !has_script() )
	{
		print_error( "HeightSourceGd error: no script assigned" );
		return Color();
	}
	ScriptInstance * si = object.ptr()->get_script_instance();
	Variant v_at     = Vector3( at.x_, at.y_, at.z_ );
	Variant v_norm   = Vector3( norm.x_, norm.y_, norm.z_ );
	Variant v_height = height;
	const Variant *ptr[3] = { &v_at, &v_norm, &v_height };
	Variant::CallError ce;
	const Variant v_ret = si->call( "color", ptr, 3, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
	{
		print_error( String( "HeightSourceGd error: argument # " ) + String::num( ce.argument ) );
		return Color();
	}
	const Color ret = v_ret;
	return ret;
}


}






