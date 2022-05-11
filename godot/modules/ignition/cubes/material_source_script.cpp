
#include "material_source_script.h"
#include "core/print_string.h"
#include "core/script_language.h"

namespace Ign
{

MaterialSourceScript::MaterialSourceScript()
	: MaterialSource()
{
	reference = nullptr;
}

MaterialSourceScript::~MaterialSourceScript()
{
}

bool MaterialSourceScript::has_script() const
{
	if ( reference == nullptr )
		return false;
	const bool has = (reference->get_script_instance() != nullptr);
	return has;
}

int MaterialSourceScript::material( const Vector3d & at ) const
{
	if ( !has_script() )
	{
		print_error( "MaterialSourceScript error: no script assigned" );
		return 0.0;
	}

	ScriptInstance * si = reference->get_script_instance();
	Variant v_at = Vector3( at.x_, at.y_, at.z_ );
	const Variant *ptr[1] = { &v_at };
	Variant::CallError ce;
	const Variant v_ret = si->call( "material", ptr, 1, ce );
	if ( ce.error != Variant::CallError::CALL_OK )
	{
		print_error( "MaterialSourceScript error: expected Vector3 as an argument" );
		return 0.0;
	}
	const int ret = v_ret;
	return ret;
}


}






