#ifndef LROCKSDB_WRITEBATCH_H
#define LROCKSDB_WRITEBATCH_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
extern "C" {
#include <luajit/lauxlib.h>
}
#include "lrocksdb/db_db.h"

lrocksdb_writebatch_t *lrocksdb_get_writebatch(lua_State *L, int index);
int lrocksdb_writebatch_reg(lua_State *L);
int lrocksdb_writebatch_create(lua_State *L);
#endif

