project(speedcrunch)

IF( APPLE )
  set( PROGNAME Speedcrunch )
  set( MACOSX_BUNDLE_ICON_FILE Speedcrunch.icns )
  set( MACOSX_BUNDLE_SHORT_VERSION_STRING trunk )
  set( MACOSX_BUNDLE_VERSION trunk )
  set( MACOSX_BUNDLE_LONG_VERSION_STRING Version trunk )
  set( CMAKE_OSX_ARCHITECTURES ppc;i386 ) #Comment out if not universal binary
ELSE( APPLE )
  set( PROGNAME speedcrunch )
ENDIF( APPLE )

cmake_minimum_required(VERSION 2.4.0)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# note: change this BEFORE making a release !!
ADD_DEFINITIONS(-DSPEEDCRUNCH_VERSION=\"trunk\" )

IF( NOT WIN32 )
IF( NOT APPLE )
  # for BinReloc stuff
  ADD_DEFINITIONS( -DENABLE_BINRELOC )
ENDIF( NOT APPLE )
ENDIF( NOT WIN32 )

# sources
INCLUDE(SourceFiles.cmake)

# resources
set(speedcrunch_RESOURCES resources/crunch.qrc)

# translations
INCLUDE(Translations.cmake)

if( MINGW )
  # resource compilation for mingw
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crunchico.o
                      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/resources/crunchico.rc -o ${CMAKE_CURRENT_BINARY_DIR}/crunchico.o )
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/crunchico.o)
  SET(LINK_FLAGS -Wl,-subsystem,windows)
else(MINGW)
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} resources/crunchico.rc)
endif(MINGW)

# enable warnings
if(CMAKE_COMPILER_IS_GNUCXX )
  ADD_DEFINITIONS( -g -Wall )
endif(CMAKE_COMPILER_IS_GNUCXX )

# setup for Qt4
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# build everything
qt4_add_RESOURCES( speedcrunch_RESOURCES_SOURCES ${speedcrunch_RESOURCES} )
QT4_WRAP_UI( speedcrunch_FORMS_HEADERS ${speedcrunch_FORMS} )
QT4_WRAP_CPP( speedcrunch_HEADERS_MOC ${speedcrunch_HEADERS} )
IF( APPLE )
  ADD_EXECUTABLE( ${PROGNAME} MACOSX_BUNDLE ${speedcrunch_SOURCES} ${speedcrunch_HEADERS_MOC} ${speedcrunch_RESOURCES_SOURCES} ${speedcruch_FORMS_HEADERS} )
  ADD_CUSTOM_COMMAND( TARGET ${PROGNAME} POST_BUILD
    COMMAND mkdir ARGS ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
    COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
    COMMAND cp ARGS *.qm ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
    COMMENT Setting up translations & resources )
  #ADD_DEPENDENCIES( ${PROGNAME} Speedcrunch_RESOURCE_DIR )
ELSE( APPLE )
  ADD_EXECUTABLE( ${PROGNAME} ${speedcrunch_SOURCES} ${speedcrunch_HEADERS_MOC} ${speedcrunch_RESOURCES_SOURCES} ${speedcrunch_FORMS_HEADERS} )
ENDIF( APPLE )

add_custom_target( confclean
  COMMAND rm -rf CMakeFiles/ CMakeCache.txt cmake_install.cmake )

target_link_libraries(${PROGNAME} ${QT_LIBRARIES} ${LINK_FLAGS})

# set(EXECUTABLE_OUTPUT_PATH ${speedcrunch_BINARY_DIR}/../bin)

# install executable and translation files
# note: it will install to CMAKE_INSTALL_PREFIX, which can be set e.g
#  cmake ../trunk  -DCMAKE_INSTALL_PREFIX=/usr
install(TARGETS ${PROGNAME} DESTINATION bin)
install(FILES ${speedcrunch_TRANSLATIONS} DESTINATION share/crunch)

# required for regression tests
enable_testing()

# regression test for HMath
set(testhmath_SOURCES
math/floatcommon.c
math/floatconst.c
math/floatconvert.c
math/floaterf.c
math/floatexp.c
math/floathmath.c
math/floatgamma.c
math/floatio.c
math/floatipower.c
math/floatlog.c
math/floatlogic.c
math/floatlong.c
math/floatnum.c
math/floatpower.c
math/floatseries.c
math/floattrig.c
math/hmath.cpp
math/number.c
test/testhmath.cpp
)
add_executable(testhmath ${testhmath_SOURCES})
add_test(testhmath ${EXECUTABLE_OUTPUT_PATH}/testhmath)

# regression test for Evaluator class
set(testevaluator_SOURCES
base/evaluator.cpp
base/functions.cpp
base/settings.cpp
base/variant.cpp
bison/exprparser.c
math/floatcommon.c
math/floatconst.c
math/floatconvert.c
math/floaterf.c
math/floatexp.c
math/floathmath.c
math/floatgamma.c
math/floatio.c
math/floatipower.c
math/floatlog.c
math/floatlogic.c
math/floatlong.c
math/floatnum.c
math/floatpower.c
math/floatseries.c
math/floattrig.c
math/hmath.cpp
math/number.c
test/testevaluator.cpp
)
add_executable(testevaluator ${testevaluator_SOURCES})
target_link_libraries(testevaluator ${QT_LIBRARIES})
add_test(testevaluator ${EXECUTABLE_OUTPUT_PATH}/testevaluator)

# regression test for floatnum
set(testfloatnum_SOURCES
main/errors.h
math/floatcommon.c
math/floatconst.c
math/floatconvert.c
math/floaterf.c
math/floatexp.c
math/floathmath.c
math/floatgamma.c
math/floatio.c
math/floatipower.c
math/floatlog.c
math/floatlogic.c
math/floatlong.c
math/floatnum.c
math/floatpower.c
math/floatseries.c
math/floattrig.c
math/number.c
test/testfloatnum.c
)
add_executable(testfloatnum ${testfloatnum_SOURCES})
add_test(testfloatnum ${EXECUTABLE_OUTPUT_PATH}/testfloatnum)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} 3rdparty/gui/wwwidgets 3rdparty/util base gui main math)
