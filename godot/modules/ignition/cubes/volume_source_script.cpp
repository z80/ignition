
#include "volume_source_script.h"
#include "core/string/print_string.h"
#include "core/object/script_language.h"
#include "core/variant/callable.h"


namespace Ign
{

VolumeSourceScript::VolumeSourceScript()
	: VolumeSource()
{
	reference = nullptr;
}

VolumeSourceScript::~VolumeSourceScript()
{
}

bool VolumeSourceScript::has_script() const
{
	if ( reference == nullptr )
		return false;
	const bool has = (reference->get_script_instance() != nullptr);
	return has;
}

void VolumeSourceScript::set_bounding_radius( Float r )
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return;
	}

	ScriptInstance * si = reference->get_script_instance();
	Variant v = static_cast<real_t>( r );
	const Variant *ptr[1] = { &v };
	Callable::CallError ce;
	const Variant v_ret = si->callp( "set_bounding_radius", ptr, 1, ce );
	if ( ce.error != Callable::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: set_bounding_radius expected real_t as an argument" );
		return;
	}
}

Float VolumeSourceScript::get_bounding_radius() const
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	Callable::CallError ce;
	const Variant v_ret = si->callp( "get_bounding_radius", nullptr, 0, ce );
	if ( ce.error != Callable::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: get_bounding_radius" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}

Float VolumeSourceScript::value( const Vector3d & at )
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	Variant v_at = Vector3( at.x_, at.y_, at.z_ );
	const Variant *ptr[1] = { &v_at };
	Callable::CallError ce;
	const Variant v_ret = si->callp( "value", ptr, 1, ce );
	if ( ce.error != Callable::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: value expected Vector3 as an argument" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}


Float VolumeSourceScript::max_node_size() const
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	const Variant *ptr[1] = { 0 };
	Callable::CallError ce;
	const Variant v_ret = si->callp( "max_node_size", ptr, 0, ce );
	if ( ce.error != Callable::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: max_node_size call failed" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}

Float VolumeSourceScript::min_node_size() const
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	const Variant *ptr[1] = { 0 };
	Callable::CallError ce;
	const Variant v_ret = si->callp( "min_node_size", ptr, 0, ce );
	if ( ce.error != Callable::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: min_node_size call failed" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}

Float VolumeSourceScript::max_node_size_local( const Vector3d & at )
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	Variant v_at = Vector3( at.x_, at.y_, at.z_ );
	const Variant *ptr[1] = { &v_at };
	Callable::CallError ce;
	const Variant v_ret = si->callp( "max_node_size_local", ptr, 1, ce );
	if ( ce.error != Callable::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: max_node_size_local expected Vector3 as an argument" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}

Float VolumeSourceScript::min_node_size_local( const Vector3d & at )
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	Variant v_at = Vector3( at.x_, at.y_, at.z_ );
	const Variant *ptr[1] = { &v_at };
	Callable::CallError ce;
	const Variant v_ret = si->callp( "min_node_size_local", ptr, 1, ce );
	if ( ce.error != Callable::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: max_node_size_local expected Vector3 as an argument" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}

int VolumeSourceScript::material( const Vector3d & at, int * priority )
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	Variant v_at = Vector3( at.x_, at.y_, at.z_ );
	const Variant *ptr[1] = { &v_at };
	Callable::CallError ce;
	const Variant v_ret = si->callp( "material", ptr, 1, ce );
	if ( ce.error != Callable::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: expected Vector3 as an argument" );
		return 0;
	}
	const int ret = v_ret;

	if ( priority != nullptr )
	{
		ScriptInstance * si = reference->get_script_instance();
		const Variant *ptr[1] = { 0 };
		Callable::CallError ce;
		const Variant v_ret = si->callp( "priority", ptr, 0, ce );
		if ( ce.error != Callable::CallError::CALL_OK )
		{
			print_error( "VolumeSourceScript error: probably not method \'material\' has been defined" );
			return 0;
		}
		const int p_ret = v_ret;
		*priority = p_ret;
	}

	return ret;
}






}



