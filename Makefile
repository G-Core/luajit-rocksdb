UNAME := $(shell uname)
all-tests := $(basename $(wildcard test/*.lua))

LIB_PATH= $(HOME)/.local/lib
INC_PATH= $(HOME)/.local/include
BIN_PATH= $(HOME)/.local/lib/bin
LUA_LIB= -L$(LIB_PATH) -lluajit-5.1
LUA_INC= -I$(INC_PATH)
ROCKSDB_INC= -I./include
ROCKSDB_LIB= -lrocksdb
#EXTRACFLAGS= -std=c99 -fPIC
EXTRACFLAGS= -fpic
# change these based on your installation
ifeq ($(UNAME), Darwin)
	LIB_PATH= /usr/local/lib
	INC_PATH= /usr/local/include
	BIN_PATH= /usr/local/bin
	LUA_LIB= -L$(LIB_PATH) -llua5.1 
	LUA_INC= -I$(INC_PATH) -I$(INC_PATH)/lua-5.1
	EXTRACFLAGS= -std=c99 -undefined dynamic_lookup -fPIC
endif


INC= $(LUA_INC) $(ROCKSDB_INC)
LIB= $(LUA_LIB) $(ROCKSDB_LIB)
WARN= -Wall
CFLAGS= -O2 $(WARN) $(INC)  -fvisibility=hidden

MYNAME= rocksdb
MYLIB= 
T= $(MYNAME).so
OBJS= src/db.o \
	  		src/cf.o \
			src/helpers.o \
			src/options.o \
			src/backup_engine.o \
			src/writebatch_cf.o \
			src/writebatch.o \
			src/iter.o

all: $T

%.o: %.cc
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

$T:	$(OBJS)
	$(CC) $(CFLAGS)  $(EXTRACFLAGS) -shared -o $@  $(LIB)  $(OBJS) -lrocksdb  

clean:
	rm -f $T $(OBJS)

tests: $(all-tests)

test/test_%: $T
	lua $@.lua

test: clean all tests

install: $(TARGET)


