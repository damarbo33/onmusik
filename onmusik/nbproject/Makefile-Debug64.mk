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
CND_PLATFORM=MinGW64-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug64
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/5f719cc8/BaseFrontend.o \
	${OBJECTDIR}/_ext/5f719cc8/Icogestor.o \
	${OBJECTDIR}/_ext/5f719cc8/Ioutil.o \
	${OBJECTDIR}/_ext/511e4115/Iofrontend.o \
	${OBJECTDIR}/_ext/511e4115/Transcode.o \
	${OBJECTDIR}/_ext/511e4115/jukebox.o \
	${OBJECTDIR}/_ext/511e4115/main.o \
	${OBJECTDIR}/_ext/e0359366/scrapper.o \
	${OBJECTDIR}/_ext/e14879cf/updater.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-fpermissive
CXXFLAGS=-fpermissive

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../../ExternalLibs/crosslib/crosslib/dist/Debug64/MinGW64-Windows -lcrosslib -mwindows -lmingw32 -lSDLmain -lSDL -lSDL_mixer -lSDL_ttf -lSDL_image -lavutil -lavformat -lavcodec -lswscale -lswresample -ljsoncpp -lsdl_gfx -lcurl -lws2_32 -lgumbo -ltinyxml -ltidy -lz -lminizip

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/onmusik.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/onmusik.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/onmusik ${OBJECTFILES} ${LDLIBSOPTIONS} resource64.res

${OBJECTDIR}/_ext/5f719cc8/BaseFrontend.o: ../../../ExternalLibs/crosslib/src/uiobjects/common/BaseFrontend.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5f719cc8
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/rijndael -I../../../ExternalLibs/crosslib/src/audioplayer -I../src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5f719cc8/BaseFrontend.o ../../../ExternalLibs/crosslib/src/uiobjects/common/BaseFrontend.cpp

${OBJECTDIR}/_ext/5f719cc8/Icogestor.o: ../../../ExternalLibs/crosslib/src/uiobjects/common/Icogestor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5f719cc8
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/rijndael -I../../../ExternalLibs/crosslib/src/audioplayer -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5f719cc8/Icogestor.o ../../../ExternalLibs/crosslib/src/uiobjects/common/Icogestor.cpp

${OBJECTDIR}/_ext/5f719cc8/Ioutil.o: ../../../ExternalLibs/crosslib/src/uiobjects/common/Ioutil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5f719cc8
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/rijndael -I../../../ExternalLibs/crosslib/src/audioplayer -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5f719cc8/Ioutil.o ../../../ExternalLibs/crosslib/src/uiobjects/common/Ioutil.cpp

: ../resource.rc
	@echo Performing Custom Build Step
	windres resource.rc -O coff -o resource.res

${OBJECTDIR}/_ext/511e4115/Iofrontend.o: ../src/Iofrontend.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/rijndael -I../../../ExternalLibs/crosslib/src/audioplayer -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/Iofrontend.o ../src/Iofrontend.cpp

${OBJECTDIR}/_ext/511e4115/Transcode.o: ../src/Transcode.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/rijndael -I../../../ExternalLibs/crosslib/src/audioplayer -I../src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/Transcode.o ../src/Transcode.cpp

${OBJECTDIR}/_ext/511e4115/jukebox.o: ../src/jukebox.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/rijndael -I../../../ExternalLibs/crosslib/src/audioplayer -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/jukebox.o ../src/jukebox.cpp

${OBJECTDIR}/_ext/511e4115/main.o: ../src/main.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/rijndael -I../../../ExternalLibs/crosslib/src/audioplayer -I../src -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/main.o ../src/main.cpp

${OBJECTDIR}/_ext/e0359366/scrapper.o: ../src/scrapper/scrapper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/e0359366
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/rijndael -I../../../ExternalLibs/crosslib/src/audioplayer -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e0359366/scrapper.o ../src/scrapper/scrapper.cpp

${OBJECTDIR}/_ext/e14879cf/updater.o: ../src/updater/updater.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/e14879cf
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/rijndael -I../../../ExternalLibs/crosslib/src/audioplayer -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e14879cf/updater.o ../src/updater/updater.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} 

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
