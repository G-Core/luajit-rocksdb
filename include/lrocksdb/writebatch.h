#ifndef LROCKSDB_WRITEBATCH_H
#define LROCKSDB_WRITEBATCH_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
extern "C" {
#ifdef USE_LUAJIT
#include <luajit-2.1/lauxlib.h>
#else
#include <lauxlib.h>
#endif
}
#include "lrocksdb/db_db.h"

lrocksdb_writebatch_cf_t *lrocksdb_get_writebatch_cf(lua_State *L, int index);
lrocksdb_writebatch_t *lrocksdb_get_writebatch(lua_State *L, int index);
int lrocksdb_writebatch_reg(lua_State *L);
int lrocksdb_writebatch_create(lua_State *L);
int lrocksdb_writebatch_cf_reg(lua_State *L);
int lrocksdb_writebatch_cf_create(lua_State *L, rocksdb_column_family_handle_t*);
#endif

