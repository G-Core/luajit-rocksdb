#include "lrocksdb/writebatch.h"

namespace {

    int put(lua_State *L);
    int clear(lua_State *L);
    int count(lua_State *L);
    int merge(lua_State *L);
    int destroy(lua_State *L);

    const struct luaL_Reg writebatch_reg[] = {
        { "put", put },
        { "clear", clear },
        { "count", count },
        { "merge", merge },
        { "destroy", destroy },
        { "__gc", destroy },
        { NULL, NULL }
    };
    int destroy(lua_State *L) {
        lrocksdb_writebatch_cf_t *rb = lrocksdb_get_writebatch_cf(L, 1);
        if(rb->writebatch != NULL) {
            rocksdb_writebatch_destroy(rb->writebatch);
            rb->writebatch = NULL;
        }
        return 0;
    }

    int clear(lua_State *L) {
        lrocksdb_writebatch_cf_t *rb = lrocksdb_get_writebatch_cf(L, 1);
        rocksdb_writebatch_clear(rb->writebatch);
        return 0;
    }

    int count(lua_State *L) {
        lrocksdb_writebatch_cf_t *rb = lrocksdb_get_writebatch_cf(L, 1);
        int count = rocksdb_writebatch_count(rb->writebatch);
        lua_pushnumber(L, count);
        return 1;
    }

    int put(lua_State *L) {
        lrocksdb_writebatch_cf_t *rb = lrocksdb_get_writebatch_cf(L, 1);
        size_t klen, vlen;
        const char *key = luaL_checklstring(L, 2, &klen);
        const char *val = luaL_checklstring(L, 3, &vlen);
        rocksdb_writebatch_put_cf(rb->writebatch, rb->handle, key, klen, val, vlen);
        return 0;
    }

    int merge(lua_State *L) {
        lrocksdb_writebatch_cf_t *rb = lrocksdb_get_writebatch_cf(L, 1);
        size_t klen, vlen;
        const char *key = luaL_checklstring(L, 2, &klen);
        const char *val = luaL_checklstring(L, 3, &vlen);
        rocksdb_writebatch_merge_cf(rb->writebatch, rb->handle, key, klen, val, vlen);
        return 0;
    }
}
    lrocksdb_writebatch_cf_t *lrocksdb_get_writebatch_cf(lua_State *L, int index) {
        lrocksdb_writebatch_cf_t *o = (lrocksdb_writebatch_cf_t*)
            luaL_checkudata(L, index, "writebatch_cf");
        luaL_argcheck(L, o != NULL && o->writebatch!= NULL, index, "writebatch_cf expected");
        return o;
    }
int lrocksdb_writebatch_cf_reg(lua_State *L) {
  lrocksdb_createmeta(L, "writebatch_cf", writebatch_reg);
  return 1;
}


int lrocksdb_writebatch_cf_create(lua_State *L, rocksdb_column_family_handle_t* h) {
  lrocksdb_writebatch_cf_t *rb =
    (lrocksdb_writebatch_cf_t *) lua_newuserdata(L, sizeof(lrocksdb_writebatch_cf_t));
  rb->writebatch = rocksdb_writebatch_create();
  rb->handle = h;
  lrocksdb_setmeta(L, "writebatch_cf");
  return 1;
}

