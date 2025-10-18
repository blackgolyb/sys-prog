PROJECT_NAME=lab2
BIN_NAME=main

all: build


build: src/*
	mkdir -p build && cmake -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++ && ninja -C build

clean:
	ninja -C build clean

.SILENT: run
run: build
	echo; echo; echo "---------------$(PROJECT_NAME)---------------"; ./build/$(BIN_NAME)

.PHONY: all build clean run
