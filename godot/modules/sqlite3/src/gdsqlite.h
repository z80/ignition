
#ifndef GDSQLITE_H
#define GDSQLITE_H

//#include <Godot.hpp>
//#include <Reference.hpp>
//#include <FuncRef.hpp>
//#include <ProjectSettings.hpp>
//#include <JSON.hpp>
//#include <JSONParseResult.hpp>

#include "core/reference.h"
#include "core/func_ref.h"



#include <fstream>
#include <vector>
#include <sstream>
#include "sqlite/sqlite3.h"

namespace godot {

struct table_struct
{
	String name, sql;
    Array row_array;
};

class SQLite : public Reference {
    GDCLASS(SQLite, Reference);

private:
    String path;
    bool verbose_mode;
    bool foreign_keys;
    std::vector<Ref<FuncRef>> function_registry;

    Dictionary deep_copy(Dictionary p_dict);
    bool validate_json( const Array & import_json, std::vector<table_struct> &tables_to_import);

public:
    sqlite3 *db;
    String error_message;
    Array query_result;

    static void _register_methods();

    SQLite();
    ~SQLite();

    void _init();

    bool open_db();
    bool import_from_json( const String & import_path );
    bool export_to_json( const String & export_path );
    void close_db();
    bool query( const String & p_query );

    bool create_table( const String & p_name, const Dictionary & p_table_dict );
    bool drop_table( const String & p_name );

    bool insert_row( const String & p_name, const Dictionary & p_row_dict );
    bool insert_rows( const String & p_name, const Array & p_row_array );

    const Array & select_rows( const String & p_name, const String & p_conditions, const Array & p_columns_array );
    bool update_rows( const String & p_name, const String & p_conditions, const Dictionary & p_updated_row_dict);
    bool delete_rows( const String & p_name, const String & p_conditions );

    bool create_function( const String & p_name, Ref<FuncRef> p_func_ref, int p_argc );

    void set_path( const String & p_path );
    const String & get_path() const;

	void set_verbose_mode( bool en );
	bool get_verbose_mode() const;

	void set_foreign_keys( bool en );
	bool get_foreign_keys() const;

	void set_error_message( const String & msg );
	const String & get_error_message() const;

	void set_query_result( const Array & data );
	const Array & get_query_result() const;
};

}

#endif


