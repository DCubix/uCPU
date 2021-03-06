#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/ucpu_asm.o \
	${OBJECTDIR}/ucpu_core.o \
	${OBJECTDIR}/ucpu_ops.o \
	${OBJECTDIR}/ucpu_types.o \
	${OBJECTDIR}/ucpu_video.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=--64

# Link Libraries and Options
LDLIBSOPTIONS=-lmingw32 `pkg-config --libs sdl2`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ucpu.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ucpu.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ucpu ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG `pkg-config --cflags sdl2` -std=c11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/ucpu_asm.o: ucpu_asm.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG `pkg-config --cflags sdl2` -std=c11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ucpu_asm.o ucpu_asm.c

${OBJECTDIR}/ucpu_core.o: ucpu_core.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG `pkg-config --cflags sdl2` -std=c11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ucpu_core.o ucpu_core.c

${OBJECTDIR}/ucpu_ops.o: ucpu_ops.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG `pkg-config --cflags sdl2` -std=c11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ucpu_ops.o ucpu_ops.c

${OBJECTDIR}/ucpu_types.o: ucpu_types.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG `pkg-config --cflags sdl2` -std=c11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ucpu_types.o ucpu_types.c

${OBJECTDIR}/ucpu_video.o: ucpu_video.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG `pkg-config --cflags sdl2` -std=c11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ucpu_video.o ucpu_video.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
