#include "lrocksdb/db.h"
#define LROCKSDB_VERSION "lua-rocksdb 0.0.1"
#define LROCKSDB_COPYRIGHT "Copyright (C) 2016, Zaher Marzuq; 2022 Joe Mariadassou"
#define LROCKSDB_DESCRIPTION "RocksDB binding for Lua"
namespace {
    int reg(lua_State *L);
    int open_db(lua_State *L);
    int put(lua_State *L);
    int get(lua_State *L);
    int open_with_cf(lua_State *L);
    int put_with_cf(lua_State *L);
    int get_with_cf(lua_State *L);
    int close(lua_State *L);
    int open_for_read_only(lua_State *L);
    int remove(lua_State *L);
    int write(lua_State *L);
    int create_iterator(lua_State *L);
    int property_value(lua_State *L);
    const struct luaL_Reg  lrocksdb_db_reg[] = {
        { "put_with_cf", put_with_cf},
        { "put", put },
        { "close", close },
        { "get_with_cf", get_with_cf },
        { "get", get },
        { "delete", remove },
        { "write", write },
        { "iterator", create_iterator },
        { "property_value", property_value },
        { NULL, NULL }
    };

    const struct luaL_Reg  lrocksdb_regs[] = {
        { "db", reg },
        { "options",  lrocksdb_options_reg },
        { "writeoptions",  lrocksdb_writeoptions_reg },
        { "readoptions",  lrocksdb_readoptions_reg },
        { "backup_engine", lrocksdb_backup_engine_reg },
        { "writebatch", lrocksdb_writebatch_reg },
        { "restoreoptions", lrocksdb_restoreoptions_reg },
        { "iterator", lrocksdb_iter_reg },
        { NULL, NULL }
    };

    const struct luaL_Reg lrocksdb_funcs[] = {
        { "open_with_cf", open_with_cf },
        { "open", open_db },
        { "open_for_read_only", open_for_read_only },
        { "options", lrocksdb_options_create },
        { "writeoptions", lrocksdb_writeoptions_create },
        { "readoptions", lrocksdb_readoptions_create },
        { "backup_engine", lrocksdb_backup_engine_open },
        { "writebatch", lrocksdb_writebatch_create },
        { "restoreoptions", lrocksdb_restoreoptions_create },
        { NULL, NULL }
    };

    int reg(lua_State *L) {
        lrocksdb_createmeta(L, "db", lrocksdb_db_reg);
        return 1;
    }


    int open_db(lua_State *L) {
        lrocksdb_options_t *o = lrocksdb_get_options(L, 1);
        const char *path = luaL_checkstring(L, 2);
        char *err = NULL;
        rocksdb_t *db = rocksdb_open(o->options, path, &err);

        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }
        lrocksdb_t *d = (lrocksdb_t *) lua_newuserdata(L, sizeof(lrocksdb_t));
        d->db = db;
        d->options = o;
        d->open = 1;
        d->column_count = 0;
        lrocksdb_setmeta(L, "db");

        return 1;
    }
    int open_with_cf(lua_State* L){
        int argc = lua_gettop(L);
        lrocksdb_options_t *o = lrocksdb_get_options(L, 1);
        const char *path = luaL_checkstring(L, 2);
        char *err = NULL;
        const char** cf_names = (const char**)malloc((argc-2)* sizeof (char*));
        rocksdb_options_t* *cf_opts = 
            ( rocksdb_options_t**)malloc((argc-2) *sizeof (rocksdb_options_t*)); 
        rocksdb_options_t* cf_options = rocksdb_options_create();
        for(int i=0; i<argc-2; ++i) {
            cf_opts[i] = cf_options; 
            cf_names[i] = luaL_checkstring(L,3+i);
        }
        rocksdb_column_family_handle_t** handles =
            (rocksdb_column_family_handle_t**)malloc((argc-2) *sizeof (rocksdb_column_family_handle_t*)); 
        rocksdb_t *db = rocksdb_open_column_families(
                o->options, path, argc-2, cf_names, cf_opts, handles, &err
                );
        if(err) {
            luaL_error(L, err);
            free(err);
            free(handles);
            for(int i=0; i<argc-2; ++i) 
                rocksdb_options_destroy(cf_opts[i] ); 
            free(cf_opts);
            free(cf_names);
            return 0;
        }
        free(cf_names);
        lrocksdb_t *d = (lrocksdb_t *) lua_newuserdata(L, sizeof(lrocksdb_t));
        d->db = db;
        d->options = o;
        d->open = 1;
        d->column_count = argc -2;
        d->cf_opts = cf_opts;
        d->handles = handles;
        lrocksdb_setmeta(L, "db");

        return 1;
    }

    int open_for_read_only(lua_State *L) {
        lrocksdb_options_t *o = lrocksdb_get_options(L, 1);
        const char *name = luaL_checkstring(L, 2);
        unsigned char error_if_log_file_exist = lua_toboolean(L, 3);
        char *err = NULL;
        rocksdb_t *db = rocksdb_open_for_read_only(o->options, name,
                error_if_log_file_exist, &err);

        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }

        lrocksdb_t *d = (lrocksdb_t *) lua_newuserdata(L, sizeof(lrocksdb_t));
        d->db = db;
        d->options = o;
        d->column_count = 0;
        d->open = 1;
        d->read_only = 1;
        lrocksdb_setmeta(L, "db");
        return 1;
    }
#if 0
    extern ROCKSDB_LIBRARY_API void rocksdb_put_cf(
            rocksdb_t* db, const rocksdb_writeoptions_t* options,
            rocksdb_column_family_handle_t* column_family, const char* key,
            size_t keylen, const char* val, size_t vallen, char** errptr);
#endif
    int put(lua_State *L) {
        lrocksdb_t *d = lrocksdb_get_db(L, 1);
        lrocksdb_writeoptions_t *wo = lrocksdb_get_writeoptions(L, 2);
        size_t key_len, value_len;
        const char *key, *value;
        char *err = NULL;

        key = luaL_checklstring(L, 3, &key_len);
        value = luaL_checklstring(L, 4, &value_len);
        rocksdb_put(d->db, wo->writeoptions, key, key_len, value, value_len, &err);
        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }
        return 1;
    }
    int put_with_cf(lua_State* L) {
        int argc=0;
        lrocksdb_t *d = lrocksdb_get_db(L, ++argc);
        lrocksdb_writeoptions_t *wo = lrocksdb_get_writeoptions(L,++argc);
        size_t key_len, value_len;
        const char *key, *value;
        char *err = NULL;
        int k = luaL_checkint(L,++argc);
        key = luaL_checklstring(L, ++argc, &key_len);
        value = luaL_checklstring(L, ++argc, &value_len);
        rocksdb_put_cf(d->db, wo->writeoptions, d->handles[k-1], key, key_len, value, value_len, &err);
        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }
        return 1;
    }

    int get(lua_State *L) {
        lrocksdb_t *d = lrocksdb_get_db(L, 1);
        lrocksdb_assert(L, d->open, "db is closed");
        lrocksdb_readoptions_t *ro = lrocksdb_get_readoptions(L, 2);
        size_t key_len, value_len;
        const char *key = luaL_checklstring(L, 3, &key_len);
        char *err = NULL;
        char *value = rocksdb_get(d->db, ro->readoptions, key, key_len, &value_len, &err);
        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }
        if(value != NULL) {
            lua_pushlstring(L, value, value_len);
            free(value);
        }
        else {
            lua_pushnil(L);
        }
        return 1;
    }
    int get_with_cf(lua_State* L){
        int argc = 0;
        lrocksdb_t *d = lrocksdb_get_db(L, ++argc);
        lrocksdb_assert(L, d->open, "db is closed");
        lrocksdb_readoptions_t *ro = lrocksdb_get_readoptions(L,++argc);
        int k = luaL_checkint(L, ++argc);
        size_t key_len, value_len;
        const char *key = luaL_checklstring(L,++argc, &key_len);
        char *err = NULL;
        char *value = rocksdb_get_cf(
                d->db, 
                ro->readoptions, 
                d->handles[k-1],
                key, key_len,
                &value_len, 
                &err);
        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }
        if(value != NULL) {
            lua_pushlstring(L, value, value_len);
            free(value);
        }
        else {
            lua_pushnil(L);
        }
        return 1;
    }

    int remove(lua_State *L) {
        lrocksdb_t *d = lrocksdb_get_db(L, 1);
        lrocksdb_writeoptions_t *wo = lrocksdb_get_writeoptions(L, 2);
        size_t key_len;
        const char *key;
        char *err = NULL;

        key = luaL_checklstring(L, 3, &key_len);
        rocksdb_delete(d->db, wo->writeoptions, key, key_len, &err);
        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }
        lua_pushnil(L);
        return 1;
    }

    int write(lua_State *L) {
        lrocksdb_t *d = lrocksdb_get_db(L, 1);
        lrocksdb_writeoptions_t *wo = lrocksdb_get_writeoptions(L, 2);
        lrocksdb_writebatch_t *rb = lrocksdb_get_writebatch(L, 3);
        char *err = NULL;
        rocksdb_write(d->db, wo->writeoptions, rb->writebatch, &err);
        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }
        lua_pushboolean(L, 1);
        return 1;
    }

    int close(lua_State *L) {
        lrocksdb_t *d = lrocksdb_get_db(L, 1);
        d->open = 0;
        for (unsigned int i = 0; i < d->column_count; ++i) {
            rocksdb_column_family_handle_destroy(d->handles[i]);
        }
        rocksdb_close(d->db);
        if (d->column_count > 0) {
            free(d->handles);
        rocksdb_options_destroy(d->cf_opts[0]);
            free(const_cast<rocksdb_options_t**>(d->cf_opts));
        }
        return 1;
    }

    int create_iterator(lua_State *L) {
        lrocksdb_t *d = lrocksdb_get_db(L, 1);
        lrocksdb_readoptions_t *ro = lrocksdb_get_readoptions(L, 2);
        lrocksdb_iterator_t *i = (lrocksdb_iterator_t *)
            lua_newuserdata(L, sizeof(lrocksdb_iterator_t));
        i->iter = rocksdb_create_iterator(d->db, ro->readoptions);
        lrocksdb_setmeta(L, "iterator");
        return 1;
    }

    int property_value(lua_State *L) {
        lrocksdb_t *d = lrocksdb_get_db(L, 1);
        const char* propname = luaL_checkstring(L, 2);
        char *propvalue = rocksdb_property_value(d->db, propname);
        if(propvalue != NULL) {
            lua_pushstring(L, propvalue);
            free(propvalue);
        }
        else {
            lua_pushnil(L);
        }
        return 1;
    }
}
#ifdef __cplusplus
extern "C"
#endif
DLL_PUBLIC int luaopen_rocksdb(lua_State *L) {
  lua_newtable(L);

  /* register classes */
  for(int i = 0; lrocksdb_regs[i].name != NULL; i++) {
    lrocksdb_regs[i].func(L);
  }

  luaL_setfuncs(L, lrocksdb_funcs, 0);

  lua_pushliteral(L, LROCKSDB_VERSION);
  lua_setfield(L, -2, "_VERSION");
  lua_pushliteral(L, LROCKSDB_COPYRIGHT);
  lua_setfield(L, -2, "_COPYRIGHT");
  lua_pushliteral(L, LROCKSDB_VERSION);
  lua_setfield(L, -2, "_DESCRIPTION");

  return 1;
}

    lrocksdb_t *lrocksdb_get_db(lua_State *L, int index) {
        lrocksdb_t *o = (lrocksdb_t *) luaL_checkudata(L, index, "db");
        luaL_argcheck(L, o != NULL && o->db != NULL, index, "db expected");
        return o;
    }
