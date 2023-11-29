local rocksdb = require("rocksdb")
local format = string.format

for k,v in pairs(rocksdb) do
  print(k..": "..tostring(v))
end

local options = rocksdb.options({
  increase_parallelism = 1,
  create_if_missing = true
})

assert(options.class == "options")

local db = rocksdb.open(options, "/tmp/rocksdb.test")

local writeoptions = rocksdb.writeoptions()
assert(writeoptions.class == "writeoptions")

local readoptions = rocksdb.readoptions()
assert(readoptions.class == "readoptions")

local key, value, expected_value

print("start: put")
for i = 0, 1000 do
  key = format("lrocks_db_key:%d", i)
  value = format("lrocks_db_value:%d", i)
  db:put(writeoptions, key, value)
end
print("done: put")

print("start: get")
for i = 0, 1000 do
  key = format("lrocks_db_key:%d", i)
  expected_value = format("lrocks_db_value:%d", i)
  value = db:get(readoptions, key)
  assert(value == expected_value)
end
print("done: get")
local prop = db:property_value("rocksdb.stats")
if prop ~= nil then
    print(prop)
end
print("delete: start", key)
key = "lrocks_db_key:delete_me"
db:put(writeoptions, key, "delete")
db:delete(writeoptions, key)
value = db:get(readoptions, key)
print("delete: end", key, type(value), value, "-")
assert(value == nil)
db:close()
print("closed")
local ok, res = pcall(db.get, db, readoptions, "testkey")
assert(ok == false)
collectgarbage()

local read_only_db = rocksdb.open_for_read_only(options, "/tmp/rocksdb.test", false)
assert(read_only_db)
print("start.read_only_db: get")
for i = 0, 1000 do
  key = format("lrocks_db_key:%d", i)
  expected_value = format("lrocks_db_value:%d", i)
  value = read_only_db:get(readoptions, key)
  assert(value == expected_value)
end
print("done.read_only_db: get")

ok, res = pcall(read_only_db.put, read_only_db, writeoptions, "test", "test")
assert(ok == false)

read_only_db:close()

-- open for read only once
local once_readoptions = rocksdb.readoptions({fill_cache = true})
local error_if_wal_files_exists = false
local read_only_db_once = rocksdb.open_for_read_only_once(options, "/tmp/rocksdb.test", error_if_wal_files_exists)
assert(read_only_db_once)
print("start.read_only_db_once: get")
for i = 0, 1000 do
  key = format("lrocks_db_key:%d", i)
  expected_value = format("lrocks_db_value:%d", i)
  value = read_only_db_once:get(once_readoptions, key)
  assert(value == expected_value)
end
print("done.read_only_db_once: get")

ok, res = pcall(read_only_db_once.put, read_only_db_once, writeoptions, "test", "test")
assert(ok == false)

read_only_db_once:close()

readoptions:destroy()
writeoptions:destroy()
options:destroy()

