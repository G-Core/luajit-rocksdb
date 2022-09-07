#ifndef LROCKSDB_ITER_H
#define LROCKSDB_ITER_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <luajit/lauxlib.h>
#include "lrocksdb_types.h"
#include "lrocksdb_helpers.h"
#include "lrocksdb_options.h"
#include "lrocksdb_db.h"

lrocksdb_iterator_t *lrocksdb_get_iter(lua_State *L, int index);
int lrocksdb_iter_reg(lua_State *L);
#endif

