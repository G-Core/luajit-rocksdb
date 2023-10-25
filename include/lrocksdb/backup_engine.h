#ifndef LROCKSDB_BACKUP_ENGINE_H
#define LROCKSDB_BACKUP_ENGINE_H
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

lrocksdb_backup_engine_t *lrocksdb_get_backup_engine(lua_State *L, int index);
 int lrocksdb_backup_engine_reg(lua_State *L);
 int lrocksdb_backup_engine_open(lua_State *L);
#endif

