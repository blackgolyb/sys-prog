PROJECT_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

SRC_FOLDER := $(abspath $())

ifeq ($(LAB),)
$(error LAB variable must be set)
endif

all: build-lab

build-lab:
	make -f ${PROJECT_DIR}/${LAB}/Makefile
