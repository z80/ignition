/*************************************************************************/
/*  bindings_generator.h                                                 */
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

#ifndef BINDINGS_GENERATOR_H
#define BINDINGS_GENERATOR_H

#include "core/class_db.h"
#include "dotnet_solution.h"
#include "editor/doc/doc_data.h"
#include "editor/editor_help.h"

#ifdef DEBUG_METHODS_ENABLED

#include "core/ustring.h"

class BindingsGenerator {

	struct ConstantInterface {
		String name;
		String proxy_name;
		int value;
		const DocData::ConstantDoc *const_doc;

		ConstantInterface() {}

		ConstantInterface(const String &p_name, const String &p_proxy_name, int p_value) {
			name = p_name;
			proxy_name = p_proxy_name;
			value = p_value;
		}
	};

	struct EnumInterface {
		StringName cname;
		List<ConstantInterface> constants;

		_FORCE_INLINE_ bool operator==(const EnumInterface &p_ienum) const {
			return p_ienum.cname == cname;
		}

		EnumInterface() {}

		EnumInterface(const StringName &p_cname) {
			cname = p_cname;
		}
	};

	struct PropertyInterface {
		StringName cname;
		String proxy_name;
		int index;

		StringName setter;
		StringName getter;

		const DocData::PropertyDoc *prop_doc;
	};

	struct TypeReference {
		StringName cname;
		bool is_enum;

		TypeReference() :
				is_enum(false) {
		}

		TypeReference(const StringName &p_cname) :
				cname(p_cname),
				is_enum(false) {
		}
	};

	struct ArgumentInterface {
		enum DefaultParamMode {
			CONSTANT,
			NULLABLE_VAL,
			NULLABLE_REF
		};

		TypeReference type;

		String name;
		String default_argument;
		DefaultParamMode def_param_mode;

		ArgumentInterface() {
			def_param_mode = CONSTANT;
		}
	};

	struct MethodInterface {
		String name;
		StringName cname;

		/**
		 * Name of the C# method
		 */
		String proxy_name;

		/**
		 * [TypeInterface::name] of the return type
		 */
		TypeReference return_type;

		/**
		 * Determines if the method has a variable number of arguments (VarArg)
		 */
		bool is_vararg;

		/**
		 * Virtual methods ("virtual" as defined by the Godot API) are methods that by default do nothing,
		 * but can be overridden by the user to add custom functionality.
		 * e.g.: _ready, _process, etc.
		 */
		bool is_virtual;

		/**
		 * Determines if the call should fallback to Godot's object.Call(string, params) in C#.
		 */
		bool requires_object_call;

		/**
		 * Determines if the method visibility is `internal` (visible only to files in the same assembly).
		 * Currently, we only use this for methods that are not meant to be exposed,
		 * but are required by properties as getters or setters.
		 * Methods that are not meant to be exposed are those that begin with underscore and are not virtual.
		 */
		bool is_internal;

		List<ArgumentInterface> arguments;

		const DocData::MethodDoc *method_doc;

		void add_argument(const ArgumentInterface &argument) {
			arguments.push_back(argument);
		}

		MethodInterface() {
			return_type.cname = BindingsGenerator::get_singleton()->name_cache.type_void;
			is_vararg = false;
			is_virtual = false;
			requires_object_call = false;
			is_internal = false;
			method_doc = NULL;
		}
	};

	struct TypeInterface {
		/**
		 * Identifier name for this type.
		 * Also used to format [c_out].
		 */
		String name;
		StringName cname;

		/**
		 * Identifier name of the base class.
		 */
		StringName base_name;

		/**
		 * Name of the C# class
		 */
		String proxy_name;

		ClassDB::APIType api_type;

		bool is_enum;
		bool is_object_type;
		bool is_singleton;
		bool is_reference;

		/**
		 * Used only by Object-derived types.
		 * Determines if this type is not abstract (incomplete).
		 * e.g.: CanvasItem cannot be instantiated.
		 */
		bool is_instantiable;

		/**
		 * Used only by Object-derived types.
		 * Determines if the C# class owns the native handle and must free it somehow when disposed.
		 * e.g.: Reference types must notify when the C# instance is disposed, for proper refcounting.
		 */
		bool memory_own;

		// !! The comments of the following fields make reference to other fields via square brackets, e.g.: [field_name]
		// !! When renaming those fields, make sure to rename their references in the comments

		// --- C INTERFACE ---

		static const char *DEFAULT_VARARG_C_IN;

		/**
		 * One or more statements that manipulate the parameter before being passed as argument of a ptrcall.
		 * If the statement adds a local that must be passed as the argument instead of the parameter,
		 * the name of that local must be specified with [c_arg_in].
		 * For variadic methods, this field is required and, if empty, [DEFAULT_VARARG_C_IN] is used instead.
		 * Formatting elements:
		 * %0: [c_type] of the parameter
		 * %1: name of the parameter
		 */
		String c_in;

		/**
		 * Determines the expression that will be passed as argument to ptrcall.
		 * By default the value equals the name of the parameter,
		 * this varies for types that require special manipulation via [c_in].
		 * Formatting elements:
		 * %0 or %s: name of the parameter
		 */
		String c_arg_in;

		/**
		 * One or more statements that determine how a variable of this type is returned from a function.
		 * It must contain the return statement(s).
		 * Formatting elements:
		 * %0: [c_type_out] of the return type
		 * %1: name of the variable to be returned
		 * %2: [name] of the return type
		 */
		String c_out;

		/**
		 * The actual expected type, as seen (in most cases) in Variant copy constructors
		 * Used for the type of the return variable and to format [c_in].
		 * The value must be the following depending of the type:
		 * Object-derived types: Object*
		 * Other types: [name]
		 * -- Exceptions --
		 * VarArg (fictitious type to represent variable arguments): Array
		 * float: double (because ptrcall only supports double)
		 * int: int64_t (because ptrcall only supports int64_t and uint64_t)
		 * Reference types override this for the type of the return variable: Ref<Reference>
		 */
		String c_type;

		/**
		 * Determines the type used for parameters in function signatures.
		 */
		String c_type_in;

		/**
		 * Determines the return type used for function signatures.
		 * Also used to construct a default value to return in case of errors,
		 * and to format [c_out].
		 */
		String c_type_out;

		// --- C# INTERFACE ---

		/**
		 * An expression that overrides the way the parameter is passed to the internal call.
		 * If empty, the parameter is passed as is.
		 * Formatting elements:
		 * %0 or %s: name of the parameter
		 */
		String cs_in;

		/**
		 * One or more statements that determine how a variable of this type is returned from a method.
		 * It must contain the return statement(s).
		 * Formatting elements:
		 * %0: internal method call statement
		 * %1: [cs_type] of the return type
		 * %2: [im_type_out] of the return type
		 */
		String cs_out;

		/**
		 * Type used for method signatures, both for parameters and the return type.
		 * Same as [proxy_name] except for variable arguments (VarArg) and collections (which include the namespace).
		 */
		String cs_type;

		/**
		 * Type used for parameters of internal call methods.
		 */
		String im_type_in;

		/**
		 * Type used for the return type of internal call methods.
		 */
		String im_type_out;

		const DocData::ClassDoc *class_doc;

		List<ConstantInterface> constants;
		List<EnumInterface> enums;
		List<PropertyInterface> properties;
		List<MethodInterface> methods;

		const MethodInterface *find_method_by_name(const StringName &p_cname) const {
			for (const List<MethodInterface>::Element *E = methods.front(); E; E = E->next()) {
				if (E->get().cname == p_cname)
					return &E->get();
			}

			return NULL;
		}

		const PropertyInterface *find_property_by_name(const StringName &p_cname) const {
			for (const List<PropertyInterface>::Element *E = properties.front(); E; E = E->next()) {
				if (E->get().cname == p_cname)
					return &E->get();
			}

			return NULL;
		}

		const PropertyInterface *find_property_by_proxy_name(const String &p_proxy_name) const {
			for (const List<PropertyInterface>::Element *E = properties.front(); E; E = E->next()) {
				if (E->get().proxy_name == p_proxy_name)
					return &E->get();
			}

			return NULL;
		}

	private:
		static void _init_value_type(TypeInterface &itype) {
			itype.proxy_name = itype.name;

			itype.c_type = itype.name;
			itype.cs_type = itype.proxy_name;
			itype.im_type_in = "ref " + itype.proxy_name;
			itype.im_type_out = itype.proxy_name;
			itype.class_doc = &EditorHelp::get_doc_data()->class_list[itype.proxy_name];
		}

	public:
		static TypeInterface create_value_type(const String &p_name) {
			TypeInterface itype;
			itype.name = p_name;
			itype.cname = StringName(p_name);
			_init_value_type(itype);
			return itype;
		}

		static TypeInterface create_value_type(const StringName &p_name) {
			TypeInterface itype;
			itype.name = p_name.operator String();
			itype.cname = p_name;
			_init_value_type(itype);
			return itype;
		}

		static TypeInterface create_object_type(const StringName &p_cname, ClassDB::APIType p_api_type) {
			TypeInterface itype;

			itype.name = p_cname;
			itype.cname = p_cname;
			itype.proxy_name = itype.name.begins_with("_") ? itype.name.substr(1, itype.name.length()) : itype.name;
			itype.api_type = p_api_type;
			itype.is_object_type = true;
			itype.class_doc = &EditorHelp::get_doc_data()->class_list[itype.proxy_name];

			return itype;
		}

		static void create_placeholder_type(TypeInterface &r_itype, const StringName &p_cname) {
			r_itype.name = p_cname;
			r_itype.cname = p_cname;
			r_itype.proxy_name = r_itype.name;

			r_itype.c_type = r_itype.name;
			r_itype.c_type_in = "MonoObject*";
			r_itype.c_type_out = "MonoObject*";
			r_itype.cs_type = r_itype.proxy_name;
			r_itype.im_type_in = r_itype.proxy_name;
			r_itype.im_type_out = r_itype.proxy_name;
		}

		static void postsetup_enum_type(TypeInterface &r_enum_itype) {
			// C interface is the same as that of 'int'. Remember to apply any
			// changes done here to the 'int' type interface as well

			r_enum_itype.c_arg_in = "&%s_in";
			{
				// The expected types for parameters and return value in ptrcall are 'int64_t' or 'uint64_t'.
				r_enum_itype.c_in = "\t%0 %1_in = (%0)%1;\n";
				r_enum_itype.c_out = "\treturn (%0)%1;\n";
				r_enum_itype.c_type = "int64_t";
			}
			r_enum_itype.c_type_in = "int32_t";
			r_enum_itype.c_type_out = r_enum_itype.c_type_in;

			r_enum_itype.cs_type = r_enum_itype.proxy_name;
			r_enum_itype.cs_in = "(int)%s";
			r_enum_itype.cs_out = "return (%1)%0;";
			r_enum_itype.im_type_in = "int";
			r_enum_itype.im_type_out = "int";
			r_enum_itype.class_doc = &EditorHelp::get_doc_data()->class_list[r_enum_itype.proxy_name];
		}

		TypeInterface() {

			api_type = ClassDB::API_NONE;

			is_enum = false;
			is_object_type = false;
			is_singleton = false;
			is_reference = false;
			is_instantiable = false;

			memory_own = false;

			c_arg_in = "%s";

			class_doc = NULL;
		}
	};

	struct InternalCall {
		String name;
		String im_type_out; // Return type for the C# method declaration. Also used as companion of [unique_siq]
		String im_sig; // Signature for the C# method declaration
		String unique_sig; // Unique signature to avoid duplicates in containers
		bool editor_only;

		InternalCall() {}

		InternalCall(const String &p_name, const String &p_im_type_out, const String &p_im_sig = String(), const String &p_unique_sig = String()) {
			name = p_name;
			im_type_out = p_im_type_out;
			im_sig = p_im_sig;
			unique_sig = p_unique_sig;
			editor_only = false;
		}

		InternalCall(ClassDB::APIType api_type, const String &p_name, const String &p_im_type_out, const String &p_im_sig = String(), const String &p_unique_sig = String()) {
			name = p_name;
			im_type_out = p_im_type_out;
			im_sig = p_im_sig;
			unique_sig = p_unique_sig;
			editor_only = api_type == ClassDB::API_EDITOR;
		}

		inline bool operator==(const InternalCall &p_a) const {
			return p_a.unique_sig == unique_sig;
		}
	};

	static bool verbose_output;

	OrderedHashMap<StringName, TypeInterface> obj_types;

	Map<StringName, TypeInterface> placeholder_types;
	Map<StringName, TypeInterface> builtin_types;
	Map<StringName, TypeInterface> enum_types;

	List<EnumInterface> global_enums;
	List<ConstantInterface> global_constants;

	List<InternalCall> method_icalls;
	Map<const MethodInterface *, const InternalCall *> method_icalls_map;

	List<const InternalCall *> generated_icall_funcs;

	List<InternalCall> core_custom_icalls;
	List<InternalCall> editor_custom_icalls;

	struct NameCache {
		StringName type_void;
		StringName type_int;
		StringName type_Array;
		StringName type_Dictionary;
		StringName type_Variant;
		StringName type_VarArg;
		StringName type_Object;
		StringName type_Reference;
		StringName type_String;
		StringName enum_Error;

		NameCache() {
			type_void = StaticCString::create("void");
			type_int = StaticCString::create("int");
			type_Array = StaticCString::create("Array");
			type_Dictionary = StaticCString::create("Dictionary");
			type_Variant = StaticCString::create("Variant");
			type_VarArg = StaticCString::create("VarArg");
			type_Object = StaticCString::create("Object");
			type_Reference = StaticCString::create("Reference");
			type_String = StaticCString::create("String");
			enum_Error = StaticCString::create("Error");
		}

		NameCache(const NameCache &);
		NameCache &operator=(const NameCache &);
	};

	NameCache name_cache;

	const List<InternalCall>::Element *find_icall_by_name(const String &p_name, const List<InternalCall> &p_list) {
		const List<InternalCall>::Element *it = p_list.front();
		while (it) {
			if (it->get().name == p_name) return it;
			it = it->next();
		}
		return NULL;
	}

	inline String get_unique_sig(const TypeInterface &p_type) {
		if (p_type.is_reference)
			return "Ref";
		else if (p_type.is_object_type)
			return "Obj";
		else if (p_type.is_enum)
			return "int";

		return p_type.name;
	}

	String bbcode_to_xml(const String &p_bbcode, const TypeInterface *p_itype);

	int _determine_enum_prefix(const EnumInterface &p_ienum);
	void _apply_prefix_to_enum_constants(EnumInterface &p_ienum, int p_prefix_length);

	void _generate_method_icalls(const TypeInterface &p_itype);

	const TypeInterface *_get_type_or_null(const TypeReference &p_typeref);
	const TypeInterface *_get_type_or_placeholder(const TypeReference &p_typeref);

	void _default_argument_from_variant(const Variant &p_val, ArgumentInterface &r_iarg);

	void _populate_object_type_interfaces();
	void _populate_builtin_type_interfaces();

	void _populate_global_constants();

	Error _generate_cs_type(const TypeInterface &itype, const String &p_output_file);

	Error _generate_cs_property(const TypeInterface &p_itype, const PropertyInterface &p_iprop, List<String> &p_output);
	Error _generate_cs_method(const TypeInterface &p_itype, const MethodInterface &p_imethod, int &p_method_bind_count, List<String> &p_output);

	void _generate_global_constants(List<String> &p_output);

	Error _generate_glue_method(const TypeInterface &p_itype, const MethodInterface &p_imethod, List<String> &p_output);

	Error _save_file(const String &p_path, const List<String> &p_content);

	BindingsGenerator() {}

	BindingsGenerator(const BindingsGenerator &);
	BindingsGenerator &operator=(const BindingsGenerator &);

	friend class CSharpLanguage;
	static BindingsGenerator *singleton;

public:
	Error generate_cs_core_project(const String &p_solution_dir, DotNetSolution &r_solution, bool p_verbose_output = true);
	Error generate_cs_editor_project(const String &p_solution_dir, DotNetSolution &r_solution, bool p_verbose_output = true);
	Error generate_cs_api(const String &p_output_dir, bool p_verbose_output = true);
	Error generate_glue(const String &p_output_dir);

	static uint32_t get_version();

	void initialize();

	_FORCE_INLINE_ static BindingsGenerator *get_singleton() {
		if (!singleton) {
			singleton = memnew(BindingsGenerator);
			singleton->initialize();
		}
		return singleton;
	}

	static void handle_cmdline_args(const List<String> &p_cmdline_args);
};

#endif

#endif // BINDINGS_GENERATOR_H
