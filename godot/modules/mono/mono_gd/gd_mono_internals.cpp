/*************************************************************************/
/*  gd_mono_internals.cpp                                                */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "gd_mono_internals.h"

#include "../csharp_script.h"
#include "../mono_gc_handle.h"
#include "../utils/macros.h"
#include "../utils/thread_local.h"
#include "gd_mono_class.h"
#include "gd_mono_marshal.h"
#include "gd_mono_utils.h"

#include <mono/metadata/exception.h>

namespace GDMonoInternals {

void tie_managed_to_unmanaged(MonoObject *managed, Object *unmanaged) {

	// This method should not fail

	CRASH_COND(!unmanaged);

	// All mono objects created from the managed world (e.g.: `new Player()`)
	// need to have a CSharpScript in order for their methods to be callable from the unmanaged side

	Reference *ref = Object::cast_to<Reference>(unmanaged);

	GDMonoClass *klass = GDMonoUtils::get_object_class(managed);

	CRASH_COND(!klass);

	GDMonoClass *native = GDMonoUtils::get_class_native_base(klass);

	CRASH_COND(native == NULL);

	if (native == klass) {
		// If it's just a wrapper Godot class and not a custom inheriting class, then attach a
		// script binding instead. One of the advantages of this is that if a script is attached
		// later and it's not a C# script, then the managed object won't have to be disposed.
		// Another reason for doing this is that this instance could outlive CSharpLanguage, which would
		// be problematic when using a script. See: https://github.com/godotengine/godot/issues/25621

		CSharpScriptBinding script_binding;

		script_binding.inited = true;
		script_binding.type_name = NATIVE_GDMONOCLASS_NAME(klass);
		script_binding.wrapper_class = klass;
		script_binding.gchandle = MonoGCHandle::create_strong(managed);

		Reference *kref = Object::cast_to<Reference>(unmanaged);
		if (kref) {
			// Unsafe refcount increment. The managed instance also counts as a reference.
			// This way if the unmanaged world has no references to our owner
			// but the managed instance is alive, the refcount will be 1 instead of 0.
			// See: godot_icall_Reference_Dtor(MonoObject *p_obj, Object *p_ptr)

			kref->reference();
		}

		// The object was just created, no script instance binding should have been attached
		CRASH_COND(unmanaged->has_script_instance_binding(CSharpLanguage::get_singleton()->get_language_index()));

		void *data = (void *)CSharpLanguage::get_singleton()->insert_script_binding(unmanaged, script_binding);

		// Should be thread safe because the object was just created and nothing else should be referencing it
		unmanaged->set_script_instance_binding(CSharpLanguage::get_singleton()->get_language_index(), data);

		return;
	}

	Ref<MonoGCHandle> gchandle = ref ? MonoGCHandle::create_weak(managed) : MonoGCHandle::create_strong(managed);

	Ref<CSharpScript> script = CSharpScript::create_for_managed_type(klass, native);

	CRASH_COND(script.is_null());

	ScriptInstance *si = CSharpInstance::create_for_managed_type(unmanaged, script.ptr(), gchandle);

	unmanaged->set_script_and_instance(script.get_ref_ptr(), si);

	return;
}

void unhandled_exception(MonoException *p_exc) {
	mono_unhandled_exception((MonoObject *)p_exc); // prints the exception as well
	// Too bad 'mono_invoke_unhandled_exception_hook' is not exposed to embedders
	GDMono::unhandled_exception_hook((MonoObject *)p_exc, NULL);
	GD_UNREACHABLE();
}

} // namespace GDMonoInternals
