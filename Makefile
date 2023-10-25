UNAME := $(shell uname)
all-tests := $(basename $(wildcard test/*.lua))

# PREFIX is environment variable, but if it is not set, then set the default value
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

LIB_PATH = /usr/local/lib
INC_PATH = /usr/local/include
BIN_PATH = /usr/local/lib/bin

OPENRESTY_PATH ?= /opt/gcore/openresty11991-gcdn

LUA_LIB = -L$(LIB_PATH) -L/$(OPENRESTY_PATH)/luajit/lib -lluajit-5.1
LUA_INC = -I$(INC_PATH) -I$(OPENRESTY_PATH)/luajit/include

ROCKSDB_INC = -I./include
ROCKSDB_LIB = -lrocksdb
EXTRACFLAGS = -fpic

INC = $(LUA_INC) $(ROCKSDB_INC)
LIB = $(LUA_LIB) $(ROCKSDB_LIB)
WARN = -Wall
CFLAGS = -O2 $(WARN) $(INC) -fvisibility=hidden

CXX = g++ -std=c++11

ifeq ($(USE_LUAJIT),1)
    CXX += -DUSE_LUAJIT
endif

SHARED_LIB = rocksdb
T = $(SHARED_LIB).so
OBJS = src/db.o \
    src/cf.o \
    src/helpers.o \
    src/options.o \
    src/backup_engine.o \
    src/writebatch_cf.o \
    src/writebatch.o \
    src/iter.o

all: $T

%.o: %.cc
	$(CXX) $(CFLAGS) -fPIC -c -o $@ $<

$T: $(OBJS)
	$(CXX) $(CFLAGS) $(EXTRACFLAGS) -shared -o $@ $(LIB) $(OBJS) -lrocksdb

clean:
	rm -f $T $(OBJS)

tests: $(all-tests)

test/%: $T
	lua $@.lua

test: clean all tests

install: $(T)
	install -d $(DESTDIR)$(PREFIX)
	install -m 755 $(T) $(DESTDIR)$(PREFIX)

docker-test:
	docker run --rm -i -e LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH $$(docker build -q .) make USE_LUAJIT=1 test