.PHONY: clean

#
# Environment variables.
#

STATIC_BUILD ?= 0
DEBUG_BUILD ?= 0

#
# Constants.
#

PROJECT_NAME = World3D
EXE_NAME = world-3d

#
# Build directories.
#

EXE_DIR_PREFIX = bin
OBJ_DIR_PREFIX = obj
EXE_DIR =
OBJ_DIR =
ifeq (${DEBUG_BUILD}, 0)
EXE_DIR += ${EXE_DIR_PREFIX}/release
OBJ_DIR += ${OBJ_DIR_PREFIX}/release
else
EXE_DIR += ${EXE_DIR_PREFIX}/debug
OBJ_DIR += ${OBJ_DIR_PREFIX}/debug
endif
EXE = ${EXE_DIR}/${EXE_NAME}

#
# Source directories.
#

SRC_DIR = src
LIB_DIR = lib
INCL_DIR = include
RES_DIR = res

#
# Source dependencies & object targets.
#

HEADERS = $(wildcard ${SRC_DIR}/*.hxx)
SOURCES = $(wildcard ${SRC_DIR}/*.cxx)
OBJECTS = $(patsubst ${SRC_DIR}/%.cxx, ${OBJ_DIR}/%.o, ${SOURCES})
DEPENDS = $(wildcard ${OBJ_DIR}/*.d)

#
# Compiler flags.
#

WARNINGS = -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wunreachable-code
CXX_STD = -std=c++17
GEN_DEPS = -MMD
DEBUGS =
OPTIMIZE =
ifeq (${DEBUG_BUILD}, 0)
OPTIMIZE += -O3
else
DEBUGS += -DDEBUG -g
OPTIMIZE += -Og
endif

#
# Libraries.
#

GLFW_LIB =
ifeq (${STATIC_BUILD}, 1)
GLFW_LIB += -L"${LIB_DIR}" -l:libglfw3.a
else
GLFW_LIB += -lglfw
endif
INCLUDES = -I${INCL_DIR}
LIBS = ${GLFW_LIB}

#
# Recipes.
#

all: prebuild exe

prebuild:
	mkdir -p ${EXE_DIR}
	mkdir -p ${OBJ_DIR}

exe: ${EXE}

${EXE}: ${OBJECTS}
	${CXX} -o $@ $^ ${LIBS}

ifneq (${DEPENDS},)
include ${DEPENDS}
endif

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cxx
	${CXX} -c -o $@ $< ${GEN_DEPS} ${WARNINGS} ${DEBUGS} ${OPTIMIZE} ${CXX_STD} ${INCLUDES}

clean:
	${RM} -v ${EXE_DIR}/* ${OBJ_DIR}/*
