
#include "register_types.h"

#include "core/engine.h"
#include "core/class_db.h"

#include "gdsqlite.h"

void register_sqlite3_types()
{
    ClassDB::register_class<godot::SQLite>();
}

void unregister_sqlite3_types()
{
}




