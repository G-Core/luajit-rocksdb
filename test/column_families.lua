local rocksdb = require("rocksdb")
local format = string.format

for k,v in pairs(rocksdb) do
  print(k..": "..tostring(v))
end

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

local key, value, expected_value

print("start: put")
for i = 0, 1000 do
  key = format("lrocks_db_key:%d", i)
  value = format("lrocks_db_value:%d", i)
  db:put_with_cf(writeoptions, 1, key, value)
end
print("done: put")

print("start: get")
for i = 0, 1000 do
  key = format("lrocks_db_key:%d", i)
  expected_value = format("lrocks_db_value:%d", i)
  value = db:get_with_cf(readoptions,1, key)
  assert(value == expected_value)
end
print("done: get")
print("delete: start", key)
key = "lrocks_db_key:delete_me"
db:put_with_cf(writeoptions,1, key, "delete")
db:delete_with_cf(writeoptions, 1, key)
value = db:get_with_cf(readoptions, 1, key)
print("delete: end", key, type(value), value, "-")
assert(value == nil)
db:close()
print("closed")

collectgarbage()


readoptions:destroy()
writeoptions:destroy()
options:destroy()

