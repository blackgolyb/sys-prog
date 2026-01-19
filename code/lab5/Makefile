PROJECT_NAME=lab5
BIN_NAME=manager

all: build

clean:
	ninja -C build clean

build: clean src/*
	mkdir -p build && cmake -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++ && ninja -C build

.SILENT: run
run: build
	echo; echo; echo "---------------$(PROJECT_NAME)---------------"; ./build/$(BIN_NAME)

.PHONY: all test build clean run
