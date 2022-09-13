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
  db:put(writeoptions, "columna" , key, value)
end
print("done: put")

local iterator = db:iterator("columna",readoptions)
assert(iterator:valid() == false)
print("error", iterator:get_error())
iterator:seek_to_first()
assert(iterator:valid() == true)

local k=0
while iterator:valid() do
  key = iterator:key()
  value = iterator:value()
  iterator:next()
  k = k+1
end
assert(k == 1001)
iterator:destroy()
db:close()
print("closed")

collectgarbage()


readoptions:destroy()
writeoptions:destroy()
options:destroy()

