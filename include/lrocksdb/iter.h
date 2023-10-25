#ifndef LROCKSDB_ITER_H
#define LROCKSDB_ITER_H
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

lrocksdb_iterator_t *lrocksdb_get_iter(lua_State *L, int index);
int lrocksdb_iter_reg(lua_State *L);
#endif

