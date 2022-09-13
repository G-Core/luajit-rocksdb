print("***** start: writebatch test *****")
local rocksdb = require("rocksdb")
local format = string.format
local db_path = "/tmp/rocksdb2.test"

local options = rocksdb.options({
  increase_parallelism = 1,
  create_missing_column_families = true,
  create_if_missing = true
})

assert(options.class == "options")
local db = rocksdb.open_with_cf(options, "/tmp/rocksdb2.test", "default", "columna")

local writeoptions = rocksdb.writeoptions()
assert(writeoptions.class == "writeoptions")

local readoptions = rocksdb.readoptions()
assert(readoptions.class == "readoptions")

local batchwrite = db:begin_batchwrite("columna")
print("***** begin batch  *****")
local key, val
for i = 1,10 do
  key = format("writebatch:key:%d", i)
  val = format("writebatch:val:%d", i)
  batchwrite:put(key, val)
  print("***** item  ",i)
  assert(batchwrite:count() == i)
end
db:finalise_batchwrite(writeoptions, batchwrite)
print("***** finalise batch  *****")
batchwrite:clear()
assert(batchwrite:count() == 0)

print("***** clear batch  *****")
batchwrite:destroy()

print("***** destroy batch  *****")
local dbval = db:get(readoptions, "columna", key)
print("*****  verify batch  *****")
assert(dbval == val)
db:close()
print("***** done: writebatch test *****")

