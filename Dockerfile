FROM ubuntu:22.04

RUN apt-get update

RUN apt-get install -y \
	build-essential \
	git \
	dh-make \
	nano \
	wget \
	curl \
	cmake \
	unzip \
	sudo \
	sed \
	libtemplate-perl \
	devscripts \
	lsb-release \
	librocksdb6.11 \
	librocksdb-dev \
  lua-any \
  luajit \
  libluajit-5.1-dev

RUN mkdir /repo
WORKDIR /repo

COPY . .

RUN make USE_LUAJIT=1