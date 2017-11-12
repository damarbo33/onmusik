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
CND_PLATFORM=MinGW2_64-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug2
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/5f719cc8/Icogestor.o \
	${OBJECTDIR}/_ext/5f719cc8/Ioutil.o \
	${OBJECTDIR}/_ext/787d0a1/AudioCD_Helpers.o \
	${OBJECTDIR}/_ext/787d0a1/CAudioCD.o \
	${OBJECTDIR}/_ext/7e2b91f9/audioplayer.o \
	${OBJECTDIR}/_ext/7e2b91f9/biquad.o \
	${OBJECTDIR}/_ext/79c6d69e/Fft.o \
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
LDLIBSOPTIONS=-L../../../ExternalLibs/crosslib/crosslib/dist/Debug2/MinGW2_64-Windows -lcrosslib -lsdl_gfx -lmingw32 -lSDLmain -lSDL -lSDL_mixer -lSDL_ttf -lSDL_image -lcurl -lws2_32 -lavutil -lavformat -lavcodec -lswscale -lswresample -ljsoncpp

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/onmusik.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/onmusik.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/onmusik ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/5f719cc8/Icogestor.o: ../../../ExternalLibs/crosslib/src/uiobjects/common/Icogestor.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5f719cc8
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5f719cc8/Icogestor.o ../../../ExternalLibs/crosslib/src/uiobjects/common/Icogestor.cpp

${OBJECTDIR}/_ext/5f719cc8/Ioutil.o: ../../../ExternalLibs/crosslib/src/uiobjects/common/Ioutil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5f719cc8
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5f719cc8/Ioutil.o ../../../ExternalLibs/crosslib/src/uiobjects/common/Ioutil.cpp

${OBJECTDIR}/_ext/787d0a1/AudioCD_Helpers.o: ../../MP3Play/src/audiocd/AudioCD_Helpers.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/787d0a1
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/787d0a1/AudioCD_Helpers.o ../../MP3Play/src/audiocd/AudioCD_Helpers.cpp

${OBJECTDIR}/_ext/787d0a1/CAudioCD.o: ../../MP3Play/src/audiocd/CAudioCD.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/787d0a1
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/787d0a1/CAudioCD.o ../../MP3Play/src/audiocd/CAudioCD.cpp

${OBJECTDIR}/_ext/7e2b91f9/audioplayer.o: ../../MP3Play/src/audioplayer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/7e2b91f9
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7e2b91f9/audioplayer.o ../../MP3Play/src/audioplayer.cpp

${OBJECTDIR}/_ext/7e2b91f9/biquad.o: ../../MP3Play/src/biquad.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/7e2b91f9
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7e2b91f9/biquad.o ../../MP3Play/src/biquad.cpp

${OBJECTDIR}/_ext/79c6d69e/Fft.o: ../../MP3Play/src/fft/Fft.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/79c6d69e
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/79c6d69e/Fft.o ../../MP3Play/src/fft/Fft.cpp

${OBJECTDIR}/_ext/511e4115/Iofrontend.o: ../src/Iofrontend.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/Iofrontend.o ../src/Iofrontend.cpp

${OBJECTDIR}/_ext/511e4115/Transcode.o: ../src/Transcode.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/Transcode.o ../src/Transcode.cpp

${OBJECTDIR}/_ext/511e4115/jukebox.o: ../src/jukebox.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/jukebox.o ../src/jukebox.cpp

${OBJECTDIR}/_ext/511e4115/main.o: ../src/main.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/511e4115
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/511e4115/main.o ../src/main.cpp

${OBJECTDIR}/_ext/e0359366/scrapper.o: ../src/scrapper/scrapper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/e0359366
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e0359366/scrapper.o ../src/scrapper/scrapper.cpp

${OBJECTDIR}/_ext/e14879cf/updater.o: ../src/updater/updater.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/e14879cf
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -DWIN -DWIN32 -I../../../ExternalLibs/crosslib/src/ziputils/zlib -I../../../ExternalLibs/crosslib/src/uiobjects -I../../../ExternalLibs/crosslib/src/uiobjects/common -I../../../ExternalLibs/crosslib/src/sqllite -I../../../ExternalLibs/crosslib/src/libjpeg -I../../../ExternalLibs/crosslib/src/ziputils/unzip -I../../../ExternalLibs/crosslib/src/tidy/include -I../../../ExternalLibs/crosslib/src/tidy/src -I../../../ExternalLibs/crosslib/src/gumbo-parser-master/src -I../../../ExternalLibs/crosslib/src/httpcurl -I../../../ExternalLibs/crosslib/src/httpcurl/jsoncpp-0.10.5/include -I../../../ExternalLibs/crosslib/src/rijndael -I../../MP3Play/src -I../../BmpRLE -I../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e14879cf/updater.o ../src/updater/updater.cpp

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
