project(speedcrunch)

IF( APPLE )
  set( PROGNAME Speedcrunch )
  set( MACOSX_BUNDLE_ICON_FILE Speedcrunch.icns )
  set( MACOSX_BUNDLE_SHORT_VERSION_STRING 0.7-beta2 )
  set( MACOSX_BUNDLE_VERSION 0.7-beta2 )
  set( MACOSX_BUNDLE_LONG_VERSION_STRING Version 0.7-beta2 )
  set( CMAKE_OSX_ARCHITECTURES ppc;i386 ) #Comment out if not universal binary
ELSE( APPLE )
  set( PROGNAME speedcrunch )
ENDIF( APPLE )

cmake_minimum_required(VERSION 2.4.0)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# note: change this BEFORE making a release !!
ADD_DEFINITIONS(-DSPEEDCRUNCH_VERSION=\"trunk\" )

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
variablesdock.h
)

# forms
set(speedcrunch_FORMS listdialog.ui)

# resources
set(speedcrunch_RESOURCES crunch.qrc)

# translations
set(speedcrunch_TRANSLATIONS
crunch_cs.qm
crunch_de.qm
crunch_es.qm
crunch_fr.qm
crunch_id.qm
crunch_it.qm
crunch_pl_PL.qm
crunch_pt_BR.qm
crunch_pt.qm
crunch_ru.qm
crunch_sl.qm
)

if( MINGW )
  # resource compilation for mingw
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crunchico.o
                      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/crunchico.rc -o ${CMAKE_CURRENT_BINARY_DIR}/crunchico.o )
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/crunchico.o)
else(MINGW)
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} crunchico.rc)
endif(MINGW)

# we still need those Q3 classes
set( QT_USE_QT3SUPPORT TRUE )

if( APPLE )
  if( QT_USE_QT3SUPPORT )
    set( QT_USE_QTSQL TRUE )
    set( QT_USE_QTNETWORK TRUE )
    set( QT_USE_QTXML TRUE )
  endif( QT_USE_QT3SUPPORT )
endif( APPLE )

# enable warnings
if(CMAKE_COMPILER_IS_GNUCXX )
  ADD_DEFINITIONS( -Wall -Werror )
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

target_link_libraries(${PROGNAME} ${QT_LIBRARIES})

# set(EXECUTABLE_OUTPUT_PATH ${speedcrunch_BINARY_DIR}/../bin)

# install executable and translation files
# note: it will install to CMAKE_INSTALL_PREFIX, which can be set e.g
#  cmake ../trunk  -DCMAKE_INSTALL_PREFIX=/usr
install(TARGETS ${PROGNAME} DESTINATION bin)
install(FILES ${speedcrunch_TRANSLATIONS} DESTINATION share/crunch)

# required for regression tests
enable_testing()

# regression test for HMath
set(testhmath_SOURCES testhmath.cpp hmath.cpp number.c)
add_executable(testhmath ${testhmath_SOURCES})
add_test(testhmath ${EXECUTABLE_OUTPUT_PATH}/testhmath)

# regression test for Evaluator class
set(testevaluator_SOURCES testevaluator.cpp evaluator.cpp functions.cpp hmath.cpp number.c)
add_executable(testevaluator ${testevaluator_SOURCES})
target_link_libraries(testevaluator ${QT_LIBRARIES})
add_test(testevaluator ${EXECUTABLE_OUTPUT_PATH}/testevaluator)
