#include "lrocksdb/db.h"
#define LROCKSDB_VERSION "lua-rocksdb 0.0.1"
#define LROCKSDB_COPYRIGHT "Copyright (C) 2016, Zaher Marzuq; 2022 Joe Mariadassou"
#define LROCKSDB_DESCRIPTION "RocksDB binding for Lua"

int lrocksdb_writebatch_cf_create(lua_State *L, rocksdb_column_family_handle_t* h) ;
namespace {
    int put(lua_State *L);
    int get(lua_State *L);
    int property_value(lua_State *L);
    int create_iterator(lua_State *L);
    int close(lua_State *L);
    int remove(lua_State *L);
    int writebatch_cf_create(lua_State *L);
    int write(lua_State *L) ;
    rocksdb_column_family_handle_t *get_cf_handle(lrocksdb_cf_t* d, const char* name, int len);
    const char* column_family="column_family";
    const struct luaL_Reg  lrocksdb_cf_reg[] = {
        { "put", put},
        { "get", get},
        { "delete", remove},
        { "close", close},
        { "iterator", create_iterator},
        { "property_value", property_value},
        { "finalise_batchwrite", write},
        { "begin_batchwrite", writebatch_cf_create},
        { NULL, NULL }
    };



    int write(lua_State *L) {
        int argc=0;
        lrocksdb_cf_t *d = lrocksdb_get_cf(L, ++argc);
        lrocksdb_writeoptions_t *wo = lrocksdb_get_writeoptions(L, ++argc);
        lrocksdb_writebatch_cf_t *rb = lrocksdb_get_writebatch_cf(L, ++argc);
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
    
    int writebatch_cf_create(lua_State *L){
        int argc=0;
        lrocksdb_cf_t *d = lrocksdb_get_cf(L, ++argc);
        size_t cf_name_len;
        const char* cf_name = luaL_checklstring(L, ++argc, &cf_name_len);
        return lrocksdb_writebatch_cf_create(L, get_cf_handle(d,cf_name,cf_name_len)) ;
    }
    int put(lua_State* L) {
        int argc=0;
        lrocksdb_cf_t *d = lrocksdb_get_cf(L, ++argc);
        lrocksdb_writeoptions_t *wo = lrocksdb_get_writeoptions(L,++argc);
        size_t key_len, value_len,cf_name_len;
        char *err = NULL;
        const char* cf_name = luaL_checklstring(L, ++argc, &cf_name_len);
        const char* key = luaL_checklstring(L, ++argc, &key_len);
        const char *value = luaL_checklstring(L, ++argc, &value_len);
        rocksdb_put_cf(d->db, wo->writeoptions, get_cf_handle(d,cf_name,cf_name_len), key, key_len, value, value_len, &err);
        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }
        return 1;
    }

    int get(lua_State* L){
        int argc = 0;
        lrocksdb_cf_t *d = lrocksdb_get_cf(L, ++argc);
        lrocksdb_assert(L, d->open, "db is closed");
        lrocksdb_readoptions_t *ro = lrocksdb_get_readoptions(L,++argc);
        size_t key_len, value_len, cf_name_len;
        const char *cf_name = luaL_checklstring(L,++argc, &cf_name_len);
        const char *key = luaL_checklstring(L,++argc, &key_len);
        char *err = NULL;
        char *value = rocksdb_get_cf(
                d->db, 
                ro->readoptions, 
                get_cf_handle(d, cf_name,cf_name_len),
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

    int remove(lua_State* L){
        lrocksdb_cf_t *d = lrocksdb_get_cf(L, 1);
        lrocksdb_writeoptions_t *wo = lrocksdb_get_writeoptions(L, 2);
        char *err = NULL;
        int argc =2;
        size_t key_len,  cf_name_len;
        const char* cf_name = luaL_checklstring(L,++argc, &cf_name_len);
        const char* key = luaL_checklstring(L, ++argc, &key_len);
        rocksdb_delete_cf(d->db, wo->writeoptions, get_cf_handle(d,cf_name,cf_name_len) , key, key_len, &err);
        if(err) {
            luaL_error(L, err);
            free(err);
            return 0;
        }
        lua_pushnil(L);
        return 1;
    }

    rocksdb_column_family_handle_t *get_cf_handle(lrocksdb_cf_t* d, const char* name, int len)
    {
        rocksdb_column_family_handle_t* retval = NULL;
        for(unsigned int i = 0; i < d->column_count; ++i) {
            if (strncmp(d->cf_names[i],name,len)==0){
                retval = d->handles[i];
                break;
            }
        }
        return retval;
    }

    int close(lua_State *L) {
        lrocksdb_cf_t *d = lrocksdb_get_cf(L, 1);
        d->open = 0;
        for (unsigned int i = 0; i < d->column_count; ++i) {
            rocksdb_column_family_handle_destroy(d->handles[i]);
        }
        rocksdb_close(d->db);
        if (d->column_count > 0) {
            free(d->handles);
            free(d->cf_names);
            rocksdb_options_destroy(d->cf_opts[0]);
            free(const_cast<rocksdb_options_t**>(d->cf_opts));
        }
        return 1;
    }


    int create_iterator(lua_State *L) {
        lrocksdb_cf_t *d = lrocksdb_get_cf(L, 1);
        int argc =1;
        size_t cf_len=0;
        const char* cf_name = luaL_checklstring(L,++argc, &cf_len);
        lrocksdb_readoptions_t *ro = lrocksdb_get_readoptions(L, ++argc);
        lrocksdb_iterator_t *i = (lrocksdb_iterator_t *)
            lua_newuserdata(L, sizeof(lrocksdb_iterator_t));
        i->iter = rocksdb_create_iterator_cf(d->db, ro->readoptions,get_cf_handle(d,cf_name,cf_len));
        lrocksdb_setmeta(L, "iterator");
        return 1;
    }

    int property_value(lua_State* L){
        lrocksdb_cf_t *d = lrocksdb_get_cf(L, 1);
        int argc =1;
        size_t cf_len=0;
        const char* cf_name = luaL_checklstring(L,++argc, &cf_len);
        const char* propname = luaL_checkstring(L, ++argc);
        char *propvalue = rocksdb_property_value_cf(d->db,get_cf_handle(d,cf_name,cf_len), propname);
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
    int cf_reg(lua_State *L) {
        lrocksdb_createmeta(L, column_family, lrocksdb_cf_reg);
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
        //rocksdb_column_family_handle_t** handles =
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
        lrocksdb_cf_t *d = (lrocksdb_cf_t *) lua_newuserdata(L, sizeof(lrocksdb_cf_t));
        d->db = db;
        d->options = o;
        d->open = 1;
        d->column_count = argc -2;
        d->cf_names = cf_names;
        d->cf_opts = cf_opts;
        d->handles = handles;
        lrocksdb_setmeta(L, column_family);

        return 1;
    }

