#ifndef LROCKSDB_DB_H
#define LROCKSDB_DB_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <luajit/lauxlib.h>

#include "lrocksdb_helpers.h"
#include "lrocksdb_types.h"
#include "lrocksdb_options.h"


lrocksdb_t *lrocksdb_get_db(lua_State *L, int index);
#endif
