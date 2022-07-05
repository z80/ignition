
#include "volume_source_script.h"
#include "core/print_string.h"
#include "core/script_language.h"


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
	Variant::CallError ce;
	const Variant v_ret = si->call( "value", ptr, 1, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: expected Vector3 as an argument" );
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
	Variant::CallError ce;
	const Variant v_ret = si->call( "max_node_size", ptr, 0, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
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
	Variant::CallError ce;
	const Variant v_ret = si->call( "min_node_size", ptr, 0, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: min_node_size call failed" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}

Float VolumeSourceScript::max_node_size_local( const Vector3d & at ) const
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	Variant v_at = Vector3( at.x_, at.y_, at.z_ );
	const Variant *ptr[1] = { &v_at };
	Variant::CallError ce;
	const Variant v_ret = si->call( "max_node_size_local", ptr, 1, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: max_node_size_local expected Vector3 as an argument" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}

Float VolumeSourceScript::min_node_size_local( const Vector3d & at ) const
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	Variant v_at = Vector3( at.x_, at.y_, at.z_ );
	const Variant *ptr[1] = { &v_at };
	Variant::CallError ce;
	const Variant v_ret = si->call( "min_node_size_local", ptr, 1, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
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
	Variant::CallError ce;
	const Variant v_ret = si->call( "material", ptr, 1, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: expected Vector3 as an argument" );
		return 0;
	}
	const int ret = v_ret;

	if ( priority != nullptr )
	{
		ScriptInstance * si = reference->get_script_instance();
		const Variant *ptr[1] = { 0 };
		Variant::CallError ce;
		const Variant v_ret = si->call( "priority", ptr, 0, ce );
		if ( ce.error != Variant::CallError::CALL_OK )
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



