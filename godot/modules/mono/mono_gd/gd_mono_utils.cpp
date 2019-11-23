/*************************************************************************/
/*  gd_mono_utils.cpp                                                    */
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

#include "gd_mono_utils.h"

#include <mono/metadata/exception.h>

#include "core/os/dir_access.h"
#include "core/os/os.h"
#include "core/project_settings.h"
#include "core/reference.h"

#include "../csharp_script.h"
#include "../utils/macros.h"
#include "../utils/mutex_utils.h"
#include "gd_mono.h"
#include "gd_mono_class.h"
#include "gd_mono_marshal.h"

namespace GDMonoUtils {

MonoCache mono_cache;

#define CACHE_AND_CHECK(m_var, m_val)                             \
	{                                                             \
		m_var = m_val;                                            \
		if (!m_var) {                                             \
			ERR_EXPLAIN("Mono Cache: Member " #m_var " is null"); \
			ERR_FAIL();                                           \
		}                                                         \
	}

#define CACHE_CLASS_AND_CHECK(m_class, m_val) CACHE_AND_CHECK(GDMonoUtils::mono_cache.class_##m_class, m_val)
#define CACHE_NS_CLASS_AND_CHECK(m_ns, m_class, m_val) CACHE_AND_CHECK(GDMonoUtils::mono_cache.class_##m_ns##_##m_class, m_val)
#define CACHE_RAW_MONO_CLASS_AND_CHECK(m_class, m_val) CACHE_AND_CHECK(GDMonoUtils::mono_cache.rawclass_##m_class, m_val)
#define CACHE_FIELD_AND_CHECK(m_class, m_field, m_val) CACHE_AND_CHECK(GDMonoUtils::mono_cache.field_##m_class##_##m_field, m_val)
#define CACHE_METHOD_AND_CHECK(m_class, m_method, m_val) CACHE_AND_CHECK(GDMonoUtils::mono_cache.method_##m_class##_##m_method, m_val)
#define CACHE_METHOD_THUNK_AND_CHECK(m_class, m_method, m_val) CACHE_AND_CHECK(GDMonoUtils::mono_cache.methodthunk_##m_class##_##m_method, m_val)

void MonoCache::clear_members() {

	class_MonoObject = NULL;
	class_bool = NULL;
	class_int8_t = NULL;
	class_int16_t = NULL;
	class_int32_t = NULL;
	class_int64_t = NULL;
	class_uint8_t = NULL;
	class_uint16_t = NULL;
	class_uint32_t = NULL;
	class_uint64_t = NULL;
	class_float = NULL;
	class_double = NULL;
	class_String = NULL;
	class_IntPtr = NULL;

#ifdef DEBUG_ENABLED
	class_System_Diagnostics_StackTrace = NULL;
	methodthunk_System_Diagnostics_StackTrace_GetFrames = NULL;
	method_System_Diagnostics_StackTrace_ctor_bool = NULL;
	method_System_Diagnostics_StackTrace_ctor_Exception_bool = NULL;
#endif

	class_KeyNotFoundException = NULL;

	rawclass_Dictionary = NULL;

	class_Vector2 = NULL;
	class_Rect2 = NULL;
	class_Transform2D = NULL;
	class_Vector3 = NULL;
	class_Basis = NULL;
	class_Quat = NULL;
	class_Transform = NULL;
	class_AABB = NULL;
	class_Color = NULL;
	class_Plane = NULL;
	class_NodePath = NULL;
	class_RID = NULL;
	class_GodotObject = NULL;
	class_GodotReference = NULL;
	class_Node = NULL;
	class_Control = NULL;
	class_Spatial = NULL;
	class_WeakRef = NULL;
	class_Array = NULL;
	class_Dictionary = NULL;
	class_MarshalUtils = NULL;

#ifdef DEBUG_ENABLED
	class_DebuggingUtils = NULL;
	methodthunk_DebuggingUtils_GetStackFrameInfo = NULL;
#endif

	class_ExportAttribute = NULL;
	field_ExportAttribute_hint = NULL;
	field_ExportAttribute_hintString = NULL;
	class_SignalAttribute = NULL;
	class_ToolAttribute = NULL;
	class_RemoteAttribute = NULL;
	class_SyncAttribute = NULL;
	class_MasterAttribute = NULL;
	class_PuppetAttribute = NULL;
	class_SlaveAttribute = NULL;
	class_RemoteSyncAttribute = NULL;
	class_MasterSyncAttribute = NULL;
	class_PuppetSyncAttribute = NULL;
	class_GodotMethodAttribute = NULL;
	field_GodotMethodAttribute_methodName = NULL;

	field_GodotObject_ptr = NULL;
	field_NodePath_ptr = NULL;
	field_Image_ptr = NULL;
	field_RID_ptr = NULL;

	methodthunk_GodotObject_Dispose = NULL;
	methodthunk_Array_GetPtr = NULL;
	methodthunk_Dictionary_GetPtr = NULL;
	methodthunk_MarshalUtils_IsArrayGenericType = NULL;
	methodthunk_MarshalUtils_IsDictionaryGenericType = NULL;
	methodthunk_SignalAwaiter_SignalCallback = NULL;
	methodthunk_SignalAwaiter_FailureCallback = NULL;
	methodthunk_GodotTaskScheduler_Activate = NULL;

	task_scheduler_handle = Ref<MonoGCHandle>();
}

void MonoCache::cleanup() {

	corlib_cache_updated = false;
	godot_api_cache_updated = false;
}

#define GODOT_API_CLASS(m_class) (GDMono::get_singleton()->get_core_api_assembly()->get_class(BINDINGS_NAMESPACE, #m_class))
#define GODOT_API_NS_CLAS(m_ns, m_class) (GDMono::get_singleton()->get_core_api_assembly()->get_class(m_ns, #m_class))

void update_corlib_cache() {

	CACHE_CLASS_AND_CHECK(MonoObject, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_object_class()));
	CACHE_CLASS_AND_CHECK(bool, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_boolean_class()));
	CACHE_CLASS_AND_CHECK(int8_t, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_sbyte_class()));
	CACHE_CLASS_AND_CHECK(int16_t, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_int16_class()));
	CACHE_CLASS_AND_CHECK(int32_t, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_int32_class()));
	CACHE_CLASS_AND_CHECK(int64_t, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_int64_class()));
	CACHE_CLASS_AND_CHECK(uint8_t, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_byte_class()));
	CACHE_CLASS_AND_CHECK(uint16_t, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_uint16_class()));
	CACHE_CLASS_AND_CHECK(uint32_t, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_uint32_class()));
	CACHE_CLASS_AND_CHECK(uint64_t, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_uint64_class()));
	CACHE_CLASS_AND_CHECK(float, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_single_class()));
	CACHE_CLASS_AND_CHECK(double, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_double_class()));
	CACHE_CLASS_AND_CHECK(String, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_string_class()));
	CACHE_CLASS_AND_CHECK(IntPtr, GDMono::get_singleton()->get_corlib_assembly()->get_class(mono_get_intptr_class()));

#ifdef DEBUG_ENABLED
	CACHE_CLASS_AND_CHECK(System_Diagnostics_StackTrace, GDMono::get_singleton()->get_corlib_assembly()->get_class("System.Diagnostics", "StackTrace"));
	CACHE_METHOD_THUNK_AND_CHECK(System_Diagnostics_StackTrace, GetFrames, (StackTrace_GetFrames)CACHED_CLASS(System_Diagnostics_StackTrace)->get_method_thunk("GetFrames"));
	CACHE_METHOD_AND_CHECK(System_Diagnostics_StackTrace, ctor_bool, CACHED_CLASS(System_Diagnostics_StackTrace)->get_method_with_desc("System.Diagnostics.StackTrace:.ctor(bool)", true));
	CACHE_METHOD_AND_CHECK(System_Diagnostics_StackTrace, ctor_Exception_bool, CACHED_CLASS(System_Diagnostics_StackTrace)->get_method_with_desc("System.Diagnostics.StackTrace:.ctor(System.Exception,bool)", true));
#endif

	CACHE_CLASS_AND_CHECK(KeyNotFoundException, GDMono::get_singleton()->get_corlib_assembly()->get_class("System.Collections.Generic", "KeyNotFoundException"));

	mono_cache.corlib_cache_updated = true;
}

void update_godot_api_cache() {

	CACHE_CLASS_AND_CHECK(Vector2, GODOT_API_CLASS(Vector2));
	CACHE_CLASS_AND_CHECK(Rect2, GODOT_API_CLASS(Rect2));
	CACHE_CLASS_AND_CHECK(Transform2D, GODOT_API_CLASS(Transform2D));
	CACHE_CLASS_AND_CHECK(Vector3, GODOT_API_CLASS(Vector3));
	CACHE_CLASS_AND_CHECK(Basis, GODOT_API_CLASS(Basis));
	CACHE_CLASS_AND_CHECK(Quat, GODOT_API_CLASS(Quat));
	CACHE_CLASS_AND_CHECK(Transform, GODOT_API_CLASS(Transform));
	CACHE_CLASS_AND_CHECK(AABB, GODOT_API_CLASS(AABB));
	CACHE_CLASS_AND_CHECK(Color, GODOT_API_CLASS(Color));
	CACHE_CLASS_AND_CHECK(Plane, GODOT_API_CLASS(Plane));
	CACHE_CLASS_AND_CHECK(NodePath, GODOT_API_CLASS(NodePath));
	CACHE_CLASS_AND_CHECK(RID, GODOT_API_CLASS(RID));
	CACHE_CLASS_AND_CHECK(GodotObject, GODOT_API_CLASS(Object));
	CACHE_CLASS_AND_CHECK(GodotReference, GODOT_API_CLASS(Reference));
	CACHE_CLASS_AND_CHECK(Node, GODOT_API_CLASS(Node));
	CACHE_CLASS_AND_CHECK(Control, GODOT_API_CLASS(Control));
	CACHE_CLASS_AND_CHECK(Spatial, GODOT_API_CLASS(Spatial));
	CACHE_CLASS_AND_CHECK(WeakRef, GODOT_API_CLASS(WeakRef));
	CACHE_CLASS_AND_CHECK(Array, GODOT_API_NS_CLAS(BINDINGS_NAMESPACE_COLLECTIONS, Array));
	CACHE_CLASS_AND_CHECK(Dictionary, GODOT_API_NS_CLAS(BINDINGS_NAMESPACE_COLLECTIONS, Dictionary));
	CACHE_CLASS_AND_CHECK(MarshalUtils, GODOT_API_CLASS(MarshalUtils));

#ifdef DEBUG_ENABLED
	CACHE_CLASS_AND_CHECK(DebuggingUtils, GODOT_API_CLASS(DebuggingUtils));
#endif

	// Attributes
	CACHE_CLASS_AND_CHECK(ExportAttribute, GODOT_API_CLASS(ExportAttribute));
	CACHE_FIELD_AND_CHECK(ExportAttribute, hint, CACHED_CLASS(ExportAttribute)->get_field("hint"));
	CACHE_FIELD_AND_CHECK(ExportAttribute, hintString, CACHED_CLASS(ExportAttribute)->get_field("hintString"));
	CACHE_CLASS_AND_CHECK(SignalAttribute, GODOT_API_CLASS(SignalAttribute));
	CACHE_CLASS_AND_CHECK(ToolAttribute, GODOT_API_CLASS(ToolAttribute));
	CACHE_CLASS_AND_CHECK(RemoteAttribute, GODOT_API_CLASS(RemoteAttribute));
	CACHE_CLASS_AND_CHECK(SyncAttribute, GODOT_API_CLASS(SyncAttribute));
	CACHE_CLASS_AND_CHECK(MasterAttribute, GODOT_API_CLASS(MasterAttribute));
	CACHE_CLASS_AND_CHECK(PuppetAttribute, GODOT_API_CLASS(PuppetAttribute));
	CACHE_CLASS_AND_CHECK(SlaveAttribute, GODOT_API_CLASS(SlaveAttribute));
	CACHE_CLASS_AND_CHECK(RemoteSyncAttribute, GODOT_API_CLASS(RemoteSyncAttribute));
	CACHE_CLASS_AND_CHECK(MasterSyncAttribute, GODOT_API_CLASS(MasterSyncAttribute));
	CACHE_CLASS_AND_CHECK(PuppetSyncAttribute, GODOT_API_CLASS(PuppetSyncAttribute));
	CACHE_CLASS_AND_CHECK(GodotMethodAttribute, GODOT_API_CLASS(GodotMethodAttribute));
	CACHE_FIELD_AND_CHECK(GodotMethodAttribute, methodName, CACHED_CLASS(GodotMethodAttribute)->get_field("methodName"));

	CACHE_FIELD_AND_CHECK(GodotObject, ptr, CACHED_CLASS(GodotObject)->get_field(BINDINGS_PTR_FIELD));
	CACHE_FIELD_AND_CHECK(NodePath, ptr, CACHED_CLASS(NodePath)->get_field(BINDINGS_PTR_FIELD));
	CACHE_FIELD_AND_CHECK(RID, ptr, CACHED_CLASS(RID)->get_field(BINDINGS_PTR_FIELD));

	CACHE_METHOD_THUNK_AND_CHECK(GodotObject, Dispose, (GodotObject_Dispose)CACHED_CLASS(GodotObject)->get_method_thunk("Dispose", 0));
	CACHE_METHOD_THUNK_AND_CHECK(Array, GetPtr, (Array_GetPtr)GODOT_API_NS_CLAS(BINDINGS_NAMESPACE_COLLECTIONS, Array)->get_method_thunk("GetPtr", 0));
	CACHE_METHOD_THUNK_AND_CHECK(Dictionary, GetPtr, (Dictionary_GetPtr)GODOT_API_NS_CLAS(BINDINGS_NAMESPACE_COLLECTIONS, Dictionary)->get_method_thunk("GetPtr", 0));
	CACHE_METHOD_THUNK_AND_CHECK(MarshalUtils, IsArrayGenericType, (IsArrayGenericType)GODOT_API_CLASS(MarshalUtils)->get_method_thunk("IsArrayGenericType", 1));
	CACHE_METHOD_THUNK_AND_CHECK(MarshalUtils, IsDictionaryGenericType, (IsDictionaryGenericType)GODOT_API_CLASS(MarshalUtils)->get_method_thunk("IsDictionaryGenericType", 1));
	CACHE_METHOD_THUNK_AND_CHECK(SignalAwaiter, SignalCallback, (SignalAwaiter_SignalCallback)GODOT_API_CLASS(SignalAwaiter)->get_method_thunk("SignalCallback", 1));
	CACHE_METHOD_THUNK_AND_CHECK(SignalAwaiter, FailureCallback, (SignalAwaiter_FailureCallback)GODOT_API_CLASS(SignalAwaiter)->get_method_thunk("FailureCallback", 0));
	CACHE_METHOD_THUNK_AND_CHECK(GodotTaskScheduler, Activate, (GodotTaskScheduler_Activate)GODOT_API_CLASS(GodotTaskScheduler)->get_method_thunk("Activate", 0));

#ifdef DEBUG_ENABLED
	CACHE_METHOD_THUNK_AND_CHECK(DebuggingUtils, GetStackFrameInfo, (DebugUtils_StackFrameInfo)GODOT_API_CLASS(DebuggingUtils)->get_method_thunk("GetStackFrameInfo", 4));
#endif

	// TODO Move to CSharpLanguage::init() and do handle disposal
	MonoObject *task_scheduler = mono_object_new(SCRIPTS_DOMAIN, GODOT_API_CLASS(GodotTaskScheduler)->get_mono_ptr());
	GDMonoUtils::runtime_object_init(task_scheduler);
	mono_cache.task_scheduler_handle = MonoGCHandle::create_strong(task_scheduler);

	mono_cache.godot_api_cache_updated = true;
}

void clear_cache() {
	mono_cache.cleanup();
	mono_cache.clear_members();
}

MonoObject *unmanaged_get_managed(Object *unmanaged) {

	if (!unmanaged)
		return NULL;

	if (unmanaged->get_script_instance()) {
		CSharpInstance *cs_instance = CAST_CSHARP_INSTANCE(unmanaged->get_script_instance());

		if (cs_instance) {
			return cs_instance->get_mono_object();
		}
	}

	// If the owner does not have a CSharpInstance...

	void *data = unmanaged->get_script_instance_binding(CSharpLanguage::get_singleton()->get_language_index());

	ERR_FAIL_NULL_V(data, NULL);

	CSharpScriptBinding &script_binding = ((Map<Object *, CSharpScriptBinding>::Element *)data)->value();

	if (!script_binding.inited) {
		SCOPED_MUTEX_LOCK(CSharpLanguage::get_singleton()->get_language_bind_mutex());

		if (!script_binding.inited) { // Other thread may have set it up
			// Already had a binding that needs to be setup
			CSharpLanguage::get_singleton()->setup_csharp_script_binding(script_binding, unmanaged);

			ERR_FAIL_COND_V(!script_binding.inited, NULL);
		}
	}

	Ref<MonoGCHandle> &gchandle = script_binding.gchandle;
	ERR_FAIL_COND_V(gchandle.is_null(), NULL);

	MonoObject *target = gchandle->get_target();

	if (target)
		return target;

	CSharpLanguage::get_singleton()->release_script_gchandle(gchandle);

	// Create a new one

#ifdef DEBUG_ENABLED
	CRASH_COND(script_binding.type_name == StringName());
	CRASH_COND(script_binding.wrapper_class == NULL);
#endif

	MonoObject *mono_object = GDMonoUtils::create_managed_for_godot_object(script_binding.wrapper_class, script_binding.type_name, unmanaged);
	ERR_FAIL_NULL_V(mono_object, NULL);

	gchandle->set_handle(MonoGCHandle::new_strong_handle(mono_object), MonoGCHandle::STRONG_HANDLE);

	// Tie managed to unmanaged
	Reference *ref = Object::cast_to<Reference>(unmanaged);

	if (ref) {
		// Unsafe refcount increment. The managed instance also counts as a reference.
		// This way if the unmanaged world has no references to our owner
		// but the managed instance is alive, the refcount will be 1 instead of 0.
		// See: godot_icall_Reference_Dtor(MonoObject *p_obj, Object *p_ptr)

		ref->reference();
	}

	return mono_object;
}

void set_main_thread(MonoThread *p_thread) {
	mono_thread_set_main(p_thread);
}

void attach_current_thread() {
	ERR_FAIL_COND(!GDMono::get_singleton()->is_runtime_initialized());
	MonoThread *mono_thread = mono_thread_attach(SCRIPTS_DOMAIN);
	ERR_FAIL_NULL(mono_thread);
}

void detach_current_thread() {
	ERR_FAIL_COND(!GDMono::get_singleton()->is_runtime_initialized());
	MonoThread *mono_thread = mono_thread_current();
	ERR_FAIL_NULL(mono_thread);
	mono_thread_detach(mono_thread);
}

MonoThread *get_current_thread() {
	return mono_thread_current();
}

void runtime_object_init(MonoObject *p_this_obj) {
	GD_MONO_BEGIN_RUNTIME_INVOKE;
	// FIXME: Do not use mono_runtime_object_init, it aborts if an exception is thrown
	mono_runtime_object_init(p_this_obj);
	GD_MONO_END_RUNTIME_INVOKE;
}

GDMonoClass *get_object_class(MonoObject *p_object) {
	return GDMono::get_singleton()->get_class(mono_object_get_class(p_object));
}

GDMonoClass *type_get_proxy_class(const StringName &p_type) {
	String class_name = p_type;

	if (class_name[0] == '_')
		class_name = class_name.substr(1, class_name.length());

	GDMonoClass *klass = GDMono::get_singleton()->get_core_api_assembly()->get_class(BINDINGS_NAMESPACE, class_name);

	if (klass && klass->is_static()) {
		// A static class means this is a Godot singleton class. If an instance is needed we use Godot.Object.
		return mono_cache.class_GodotObject;
	}

#ifdef TOOLS_ENABLED
	if (!klass) {
		return GDMono::get_singleton()->get_editor_api_assembly()->get_class(BINDINGS_NAMESPACE, class_name);
	}
#endif

	return klass;
}

GDMonoClass *get_class_native_base(GDMonoClass *p_class) {
	GDMonoClass *klass = p_class;

	do {
		const GDMonoAssembly *assembly = klass->get_assembly();
		if (assembly == GDMono::get_singleton()->get_core_api_assembly())
			return klass;
#ifdef TOOLS_ENABLED
		if (assembly == GDMono::get_singleton()->get_editor_api_assembly())
			return klass;
#endif
	} while ((klass = klass->get_parent_class()) != NULL);

	return NULL;
}

MonoObject *create_managed_for_godot_object(GDMonoClass *p_class, const StringName &p_native, Object *p_object) {
	String object_type = p_object->get_class_name();

	if (object_type[0] == '_')
		object_type = object_type.substr(1, object_type.length());

	if (!ClassDB::is_parent_class(object_type, p_native)) {
		ERR_EXPLAIN("Type inherits from native type '" + p_native + "', so it can't be instanced in object of type: '" + p_object->get_class() + "'");
		ERR_FAIL_V(NULL);
	}

	MonoObject *mono_object = mono_object_new(SCRIPTS_DOMAIN, p_class->get_mono_ptr());
	ERR_FAIL_NULL_V(mono_object, NULL);

	CACHED_FIELD(GodotObject, ptr)->set_value_raw(mono_object, p_object);

	// Construct
	GDMonoUtils::runtime_object_init(mono_object);

	return mono_object;
}

MonoObject *create_managed_from(const NodePath &p_from) {
	MonoObject *mono_object = mono_object_new(SCRIPTS_DOMAIN, CACHED_CLASS_RAW(NodePath));
	ERR_FAIL_NULL_V(mono_object, NULL);

	// Construct
	GDMonoUtils::runtime_object_init(mono_object);

	CACHED_FIELD(NodePath, ptr)->set_value_raw(mono_object, memnew(NodePath(p_from)));

	return mono_object;
}

MonoObject *create_managed_from(const RID &p_from) {
	MonoObject *mono_object = mono_object_new(SCRIPTS_DOMAIN, CACHED_CLASS_RAW(RID));
	ERR_FAIL_NULL_V(mono_object, NULL);

	// Construct
	GDMonoUtils::runtime_object_init(mono_object);

	CACHED_FIELD(RID, ptr)->set_value_raw(mono_object, memnew(RID(p_from)));

	return mono_object;
}

MonoObject *create_managed_from(const Array &p_from, GDMonoClass *p_class) {
	MonoObject *mono_object = mono_object_new(SCRIPTS_DOMAIN, p_class->get_mono_ptr());
	ERR_FAIL_NULL_V(mono_object, NULL);

	// Search constructor that takes a pointer as parameter
	MonoMethod *m;
	void *iter = NULL;
	while ((m = mono_class_get_methods(p_class->get_mono_ptr(), &iter))) {
		if (strcmp(mono_method_get_name(m), ".ctor") == 0) {
			MonoMethodSignature *sig = mono_method_signature(m);
			void *front = NULL;
			if (mono_signature_get_param_count(sig) == 1 &&
					mono_class_from_mono_type(mono_signature_get_params(sig, &front)) == CACHED_CLASS(IntPtr)->get_mono_ptr()) {
				break;
			}
		}
	}

	CRASH_COND(m == NULL);

	Array *new_array = memnew(Array(p_from));
	void *args[1] = { &new_array };

	MonoException *exc = NULL;
	GDMonoUtils::runtime_invoke(m, mono_object, args, &exc);
	UNLIKELY_UNHANDLED_EXCEPTION(exc);

	return mono_object;
}

MonoObject *create_managed_from(const Dictionary &p_from, GDMonoClass *p_class) {
	MonoObject *mono_object = mono_object_new(SCRIPTS_DOMAIN, p_class->get_mono_ptr());
	ERR_FAIL_NULL_V(mono_object, NULL);

	// Search constructor that takes a pointer as parameter
	MonoMethod *m;
	void *iter = NULL;
	while ((m = mono_class_get_methods(p_class->get_mono_ptr(), &iter))) {
		if (strcmp(mono_method_get_name(m), ".ctor") == 0) {
			MonoMethodSignature *sig = mono_method_signature(m);
			void *front = NULL;
			if (mono_signature_get_param_count(sig) == 1 &&
					mono_class_from_mono_type(mono_signature_get_params(sig, &front)) == CACHED_CLASS(IntPtr)->get_mono_ptr()) {
				break;
			}
		}
	}

	CRASH_COND(m == NULL);

	Dictionary *new_dict = memnew(Dictionary(p_from));
	void *args[1] = { &new_dict };

	MonoException *exc = NULL;
	GDMonoUtils::runtime_invoke(m, mono_object, args, &exc);
	UNLIKELY_UNHANDLED_EXCEPTION(exc);

	return mono_object;
}

MonoDomain *create_domain(const String &p_friendly_name) {
	MonoDomain *domain = mono_domain_create_appdomain((char *)p_friendly_name.utf8().get_data(), NULL);

	if (domain) {
		// Workaround to avoid this exception:
		// System.Configuration.ConfigurationErrorsException: Error Initializing the configuration system.
		// ---> System.ArgumentException: The 'ExeConfigFilename' argument cannot be null.
		mono_domain_set_config(domain, ".", "");
	}

	return domain;
}

String get_exception_name_and_message(MonoException *p_exc) {
	String res;

	MonoClass *klass = mono_object_get_class((MonoObject *)p_exc);
	MonoType *type = mono_class_get_type(klass);

	char *full_name = mono_type_full_name(type);
	res += full_name;
	mono_free(full_name);

	res += ": ";

	MonoProperty *prop = mono_class_get_property_from_name(klass, "Message");
	MonoString *msg = (MonoString *)property_get_value(prop, (MonoObject *)p_exc, NULL, NULL);
	res += GDMonoMarshal::mono_string_to_godot(msg);

	return res;
}

void set_exception_message(MonoException *p_exc, String message) {
	MonoClass *klass = mono_object_get_class((MonoObject *)p_exc);
	MonoProperty *prop = mono_class_get_property_from_name(klass, "Message");
	MonoString *msg = GDMonoMarshal::mono_string_from_godot(message);
	void *params[1] = { msg };
	property_set_value(prop, (MonoObject *)p_exc, params, NULL);
}

void debug_print_unhandled_exception(MonoException *p_exc) {
	print_unhandled_exception(p_exc);
	debug_send_unhandled_exception_error(p_exc);
}

void debug_send_unhandled_exception_error(MonoException *p_exc) {
#ifdef DEBUG_ENABLED
	if (!ScriptDebugger::get_singleton())
		return;

	_TLS_RECURSION_GUARD_;

	ScriptLanguage::StackInfo separator;
	separator.file = String();
	separator.func = "--- " + RTR("End of inner exception stack trace") + " ---";
	separator.line = 0;

	Vector<ScriptLanguage::StackInfo> si;
	String exc_msg;

	while (p_exc != NULL) {
		GDMonoClass *st_klass = CACHED_CLASS(System_Diagnostics_StackTrace);
		MonoObject *stack_trace = mono_object_new(mono_domain_get(), st_klass->get_mono_ptr());

		MonoBoolean need_file_info = true;
		void *ctor_args[2] = { p_exc, &need_file_info };

		MonoException *unexpected_exc = NULL;
		CACHED_METHOD(System_Diagnostics_StackTrace, ctor_Exception_bool)->invoke_raw(stack_trace, ctor_args, &unexpected_exc);

		if (unexpected_exc) {
			GDMonoInternals::unhandled_exception(unexpected_exc);
			GD_UNREACHABLE();
		}

		Vector<ScriptLanguage::StackInfo> _si;
		if (stack_trace != NULL) {
			_si = CSharpLanguage::get_singleton()->stack_trace_get_info(stack_trace);
			for (int i = _si.size() - 1; i >= 0; i--)
				si.insert(0, _si[i]);
		}

		exc_msg += (exc_msg.length() > 0 ? " ---> " : "") + GDMonoUtils::get_exception_name_and_message(p_exc);

		GDMonoClass *exc_class = GDMono::get_singleton()->get_class(mono_get_exception_class());
		GDMonoProperty *inner_exc_prop = exc_class->get_property("InnerException");
		CRASH_COND(inner_exc_prop == NULL);

		MonoObject *inner_exc = inner_exc_prop->get_value((MonoObject *)p_exc);
		if (inner_exc != NULL)
			si.insert(0, separator);

		p_exc = (MonoException *)inner_exc;
	}

	String file = si.size() ? si[0].file : __FILE__;
	String func = si.size() ? si[0].func : FUNCTION_STR;
	int line = si.size() ? si[0].line : __LINE__;
	String error_msg = "Unhandled exception";

	ScriptDebugger::get_singleton()->send_error(func, file, line, error_msg, exc_msg, ERR_HANDLER_ERROR, si);
#endif
}

void debug_unhandled_exception(MonoException *p_exc) {
	GDMonoInternals::unhandled_exception(p_exc); // prints the exception as well
	GD_UNREACHABLE();
}

void print_unhandled_exception(MonoException *p_exc) {
	mono_print_unhandled_exception((MonoObject *)p_exc);
}

void set_pending_exception(MonoException *p_exc) {
#ifdef HAS_PENDING_EXCEPTIONS
	if (get_runtime_invoke_count() == 0) {
		debug_unhandled_exception(p_exc);
		GD_UNREACHABLE();
	}

	if (!mono_runtime_set_pending_exception(p_exc, false)) {
		ERR_PRINTS("Exception thrown from managed code, but it could not be set as pending:");
		GDMonoUtils::debug_print_unhandled_exception(p_exc);
	}
#else
	debug_unhandled_exception(p_exc);
	GD_UNREACHABLE();
#endif
}

_THREAD_LOCAL_(int)
current_invoke_count = 0;

MonoObject *runtime_invoke(MonoMethod *p_method, void *p_obj, void **p_params, MonoException **r_exc) {
	GD_MONO_BEGIN_RUNTIME_INVOKE;
	MonoObject *ret = mono_runtime_invoke(p_method, p_obj, p_params, (MonoObject **)r_exc);
	GD_MONO_END_RUNTIME_INVOKE;
	return ret;
}

MonoObject *runtime_invoke_array(MonoMethod *p_method, void *p_obj, MonoArray *p_params, MonoException **r_exc) {
	GD_MONO_BEGIN_RUNTIME_INVOKE;
	MonoObject *ret = mono_runtime_invoke_array(p_method, p_obj, p_params, (MonoObject **)r_exc);
	GD_MONO_END_RUNTIME_INVOKE;
	return ret;
}

MonoString *object_to_string(MonoObject *p_obj, MonoException **r_exc) {
	GD_MONO_BEGIN_RUNTIME_INVOKE;
	MonoString *ret = mono_object_to_string(p_obj, (MonoObject **)r_exc);
	GD_MONO_END_RUNTIME_INVOKE;
	return ret;
}

void property_set_value(MonoProperty *p_prop, void *p_obj, void **p_params, MonoException **r_exc) {
	GD_MONO_BEGIN_RUNTIME_INVOKE;
	mono_property_set_value(p_prop, p_obj, p_params, (MonoObject **)r_exc);
	GD_MONO_END_RUNTIME_INVOKE;
}

MonoObject *property_get_value(MonoProperty *p_prop, void *p_obj, void **p_params, MonoException **r_exc) {
	GD_MONO_BEGIN_RUNTIME_INVOKE;
	MonoObject *ret = mono_property_get_value(p_prop, p_obj, p_params, (MonoObject **)r_exc);
	GD_MONO_END_RUNTIME_INVOKE;
	return ret;
}

uint64_t unbox_enum_value(MonoObject *p_boxed, MonoType *p_enum_basetype, bool &r_error) {
	r_error = false;
	switch (mono_type_get_type(p_enum_basetype)) {
		case MONO_TYPE_BOOLEAN:
			return (bool)GDMonoMarshal::unbox<MonoBoolean>(p_boxed) ? 1 : 0;
		case MONO_TYPE_CHAR:
			return GDMonoMarshal::unbox<uint16_t>(p_boxed);
		case MONO_TYPE_U1:
			return GDMonoMarshal::unbox<uint8_t>(p_boxed);
		case MONO_TYPE_U2:
			return GDMonoMarshal::unbox<uint16_t>(p_boxed);
		case MONO_TYPE_U4:
			return GDMonoMarshal::unbox<uint32_t>(p_boxed);
		case MONO_TYPE_U8:
			return GDMonoMarshal::unbox<uint64_t>(p_boxed);
		case MONO_TYPE_I1:
			return GDMonoMarshal::unbox<int8_t>(p_boxed);
		case MONO_TYPE_I2:
			return GDMonoMarshal::unbox<int16_t>(p_boxed);
		case MONO_TYPE_I4:
			return GDMonoMarshal::unbox<int32_t>(p_boxed);
		case MONO_TYPE_I8:
			return GDMonoMarshal::unbox<int64_t>(p_boxed);
		default:
			r_error = true;
			return 0;
	}
}

void dispose(MonoObject *p_mono_object, MonoException **r_exc) {
	invoke_method_thunk(CACHED_METHOD_THUNK(GodotObject, Dispose), p_mono_object, (MonoObject **)r_exc);
}

} // namespace GDMonoUtils
