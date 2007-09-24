project(speedcrunch-floatnum)

IF( APPLE )
  set( PROGNAME Speedcrunch )
  set( MACOSX_BUNDLE_ICON_FILE Speedcrunch.icns )
  set( MACOSX_BUNDLE_SHORT_VERSION_STRING 0.8 )
  set( MACOSX_BUNDLE_VERSION 0.8 )
  set( MACOSX_BUNDLE_LONG_VERSION_STRING Version post-0.8 )
  set( CMAKE_OSX_ARCHITECTURES ppc;i386 ) #Comment out if not universal binary
ELSE( APPLE )
  set( PROGNAME speedcrunch )
ENDIF( APPLE )

cmake_minimum_required(VERSION 2.4.0)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# note: change this BEFORE making a release !!
ADD_DEFINITIONS(-DSPEEDCRUNCH_VERSION=\"post-0.8\" )

IF( NOT WIN32 )
IF( NOT APPLE )
  # for BinReloc stuff
  ADD_DEFINITIONS( -DENABLE_BINRELOC )
ENDIF( NOT APPLE )
ENDIF( NOT WIN32 )


# note: sort files alphabetically !

# sources
set(speedcrunch_SOURCES
aboutbox.cpp
autohidelabel.cpp
binreloc.c
configdlg.cpp
constants.cpp
constantsdock.cpp
crunch.cpp
deletevardlg.cpp
editor.cpp
evaluator.cpp
floatcommon.c
floatconst.c
floatconvert.c
floatexp.c
floatgamma.c
floathmath.c
floatio.c
floatipower.c
floatlog.c
floatlogic.c
floatlong.c
floatnum.c
floatpower.c
floatseries.c
floattrig.c
functions.cpp
functionsdock.cpp
historydock.cpp
hmath.cpp
insertfunctiondlg.cpp
insertvardlg.cpp
keypad.cpp
main.cpp
number.c
result.cpp
settings.cpp
tipwidget.cpp
variablesdock.cpp
)

# headers
set(speedcrunch_HEADERS
aboutbox.h
autohidelabel.h
binreloc.h
configdlg.h
constants.h
constantsdock.h
crunch.h
deletevardlg.h
editor.h
evaluator.h
floatcommon.h
floatconfig.h
floatconst.h
floatconvert.h
floatexp.h
floatgamma.h
floathmath.h
floatio.h
floatipower.h
floatlog.h
floatlogic.h
floatlong.h
floatnum.h
floatpower.h
floatseries.h
floattrig.h
functions.h
functionsdock.h
historydock.h
hmath.h
insertfunctiondlg.h
insertvardlg.h
keypad.h
number.h
result.h
settings.h
tipwidget.h
variablesdock.h
)

# forms
# set(speedcrunch_FORMS foobar.ui)

# resources
set(speedcrunch_RESOURCES crunch.qrc)

# translations
set(speedcrunch_TRANSLATIONS
crunch_cs.qm
crunch_de.qm
crunch_es_AR.qm
crunch_es.qm
crunch_fi.qm
crunch_fr.qm
crunch_he.qm
#crunch_id.qm
crunch_it.qm
crunch_nl.qm
crunch_no.qm
#crunch_pl.qm
crunch_pt_BR.qm
crunch_pt.qm
crunch_ro.qm
crunch_ru.qm
crunch_sv.qm
#crunch_sl.qm
)

if( MINGW )
  # resource compilation for mingw
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crunchico.o
                      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/crunchico.rc -o ${CMAKE_CURRENT_BINARY_DIR}/crunchico.o )
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/crunchico.o)
  SET(LINK_FLAGS -Wl,-subsystem,windows)
else(MINGW)
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} crunchico.rc)
endif(MINGW)

# enable warnings
if(CMAKE_COMPILER_IS_GNUCXX )
  ADD_DEFINITIONS( -Wall )
endif(CMAKE_COMPILER_IS_GNUCXX )

# setup for Qt4
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# build everything
qt4_add_RESOURCES( speedcrunch_RESOURCES_SOURCES ${speedcrunch_RESOURCES} )
QT4_WRAP_UI( speedcrunch_FORMS_HEADERS ${speedcrunch_FORMS} )
QT4_WRAP_CPP( speedcrunch_HEADERS_MOC ${speedcrunch_HEADERS} )
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
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
set(testhmath_SOURCES testhmath.cpp hmath.cpp number.c floatnum.c floatlong.c
  floatexp.c floatconst.c floatcommon.c floattrig.c floatseries.c
  floathmath.c floatlogic.c floatconvert.c floatio.c floatipower.c floatgamma.c
  floatlog.c floatpower.c)
add_executable(testhmath ${testhmath_SOURCES})
add_test(testhmath ${EXECUTABLE_OUTPUT_PATH}/testhmath)

# regression test for Evaluator class
set(testevaluator_SOURCES testevaluator.cpp evaluator.cpp functions.cpp hmath.cpp number.c
  floatnum.c floatlong.c floatexp.c floatconst.c floatcommon.c floattrig.c floatseries.c
  floathmath.c floatlogic.c floatconvert.c floatio.c floatipower.c floatgamma.c
  floatlog.c floatpower.c)
add_executable(testevaluator ${testevaluator_SOURCES})
target_link_libraries(testevaluator ${QT_LIBRARIES})
add_test(testevaluator ${EXECUTABLE_OUTPUT_PATH}/testevaluator)

# regression test for floatnum
set(testfloatnum_SOURCES testfloatnum.c floatnum.c number.c floatcommon.c
 floatconst.c floatseries.c floatlog.c floatexp.c floattrig.c floatlong.c
 floatipower.c floatpower.c floathmath.c floatconvert.c floatgamma.c
 floatlogic.c floatio.c)
add_executable(testfloatnum ${testfloatnum_SOURCES})
add_test(testfloatnum ${EXECUTABLE_OUTPUT_PATH}/testfloatnum)
