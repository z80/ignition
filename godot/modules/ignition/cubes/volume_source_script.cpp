
#include "volume_source_script.h"
#include "core/print_string.h"
#include "core/script_language.h"


namespace Ign
{

VolumeSourceScript::VolumeSourceScript()
	: VolumeSource()
{
}

VolumeSourceScript::~VolumeSourceScript()
{
}

bool VolumeSourceScript::has_script() const
{
	if ( object.ptr() == nullptr )
		return false;
	const bool has = (object.ptr()->get_script_instance() != nullptr);
	return has;
}

Float VolumeSourceScript::value( const Vector3d & at ) const
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = object.ptr()->get_script_instance();
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

	ScriptInstance * si = object.ptr()->get_script_instance();
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

	ScriptInstance * si = object.ptr()->get_script_instance();
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

Float VolumeSourceScript::max_node_size_at( const Vector3d & at ) const
{
	if ( !has_script() )
	{
		print_error( "VolumeSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = object.ptr()->get_script_instance();
	Variant v_at = Vector3( at.x_, at.y_, at.z_ );
	const Variant *ptr[1] = { &v_at };
	Variant::CallError ce;
	const Variant v_ret = si->call( "max_node_size_at", ptr, 1, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
	{
		print_error( "VolumeSourceScript error: max_node_size_at expected Vector3 as an argument" );
		return 0.0;
	}
	const real_t ret = v_ret;
	return ret;
}




}



