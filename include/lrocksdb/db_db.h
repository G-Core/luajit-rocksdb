#ifndef LROCKSDB_DB_H
#define LROCKSDB_DB_H
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
#include "lrocksdb/helpers.h"
#include "lrocksdb/types.h"
#include "lrocksdb/options.h"


    int cf_reg(lua_State *L);
    int open_with_cf(lua_State *L);
lrocksdb_cf_t *lrocksdb_get_cf(lua_State *L, int index);
lrocksdb_t *lrocksdb_get_db(lua_State *L, int index);
#endif
