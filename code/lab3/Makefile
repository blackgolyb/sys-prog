PROJECT_NAME=lab3
BIN_NAME=main

all: build

clean:
	ninja -C build clean

build: clean src/*
	mkdir -p build && cmake -B build -G Ninja -DCMAKE_C_COMPILER=clang && ninja -C build

.SILENT: run
run: build
	echo; echo; echo "---------------$(PROJECT_NAME)---------------"; ./build/$(BIN_NAME)

.PHONY: all test build clean run
