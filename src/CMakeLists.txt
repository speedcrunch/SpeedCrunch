PROJECT(speedcrunch)

cmake_minimum_required(VERSION 2.4.4)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy)

set(CMAKE_COLOR_MAKEFILE      ON)
set(CMAKE_VERBOSE_MAKEFILE    OFF)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(QT_MIN_VERSION "4.8.0")

SET( speedcrunch_VERSION "master" )

IF( APPLE )
  SET( PROGNAME SpeedCrunch )
  SET( MACOSX_BUNDLE_ICON_FILE speedcrunch.icns )
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING ${speedcrunch_VERSION} )
  SET( MACOSX_BUNDLE_VERSION ${speedcrunch_VERSION} )
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version ${speedcrunch_VERSION} )
  SET( CMAKE_OSX_ARCHITECTURES ppc;i386 )
ELSE( APPLE )
  SET( PROGNAME speedcrunch )
ENDIF( APPLE )

ADD_DEFINITIONS("-DSPEEDCRUNCH_VERSION=\\\"${speedcrunch_VERSION}\\\"")
ADD_DEFINITIONS(-DQT_USE_QSTRINGBUILDER)

# sources
INCLUDE(SourceFiles.cmake)

IF( MINGW )
  # resource compilation for mingw
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o
                      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/resources/speedcrunch.rc -o ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o )
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o)
ELSE(MINGW)
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} resources/speedcrunch.rc)
ENDIF(MINGW)

# Enable all warnings and C++11 required flags on some compilers.
string(COMPARE EQUAL "${CMAKE_CXX_COMPILER_ID}" MSVC USING_MSVC)
if(USING_MSVC)
    add_definitions(/Wall)
else()
    add_definitions(-Wall)
    list(APPEND CMAKE_CXX_FLAGS "-std=c++0x")
endif()

# setup for Qt4
SET(QT_USE_QTNETWORK TRUE)
#SET(QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# build everything
qt4_add_RESOURCES( speedcrunch_RESOURCES_SOURCES ${speedcrunch_RESOURCES} )
QT4_WRAP_UI( speedcrunch_FORMS_HEADERS ${speedcrunch_FORMS} )
QT4_WRAP_CPP( speedcrunch_HEADERS_MOC ${speedcrunch_HEADERS} )
ADD_EXECUTABLE( ${PROGNAME} WIN32 MACOSX_BUNDLE ${speedcrunch_SOURCES} ${speedcrunch_HEADERS_MOC} ${speedcrunch_RESOURCES_SOURCES} ${speedcrunch_FORMS_HEADERS} )

IF( APPLE )
  SET( speedcrunch_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources )
  ADD_CUSTOM_COMMAND( TARGET ${PROGNAME} POST_BUILD
    COMMAND mkdir ARGS -p ${speedcrunch_RESOURCE_DIR}
    COMMAND cp ARGS -f resources/${MACOSX_BUNDLE_ICON_FILE} ${speedcrunch_RESOURCE_DIR} )
ENDIF( APPLE )

ADD_CUSTOM_TARGET( confclean COMMAND rm -rf Makefile CMakeFiles/ CMakeCache.txt cmake_install.cmake DartTestfile.txt install_manifest.txt )

find_library(x11 X11)
IF(x11)
  TARGET_LINK_LIBRARIES(${PROGNAME} ${QT_LIBRARIES} X11)
ELSE(x11)
  TARGET_LINK_LIBRARIES(${PROGNAME} ${QT_LIBRARIES})
ENDIF(x11)

# only needed for static builds when directx is enabled in qt and you
# get a linker error because of missing a directx function
#IF(WIN32)
#  TARGET_LINK_LIBRARIES( speedcrunch dxguid )
#ENDIF(WIN32)

# set(EXECUTABLE_OUTPUT_PATH ${speedcrunch_BINARY_DIR}/../bin)

# required for regression tests
ENABLE_TESTING()

ADD_EXECUTABLE(testhmath ${testhmath_SOURCES})
ADD_TEST(testhmath testhmath)

QT4_WRAP_CPP(testevaluator_HEADERS_MOC ${testevaluator_HEADERS})
ADD_EXECUTABLE(testevaluator ${testevaluator_SOURCES} ${testevaluator_HEADERS_MOC})
TARGET_LINK_LIBRARIES(testevaluator ${QT_LIBRARIES})
ADD_TEST(testevaluator testevaluator)

ADD_EXECUTABLE(testfloatnum ${testfloatnum_SOURCES})
ADD_TEST(testfloatnum testfloatnum)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} thirdparty core gui math)

################################# INSTALL ######################################

# macros, definitions and options
IF(NOT WIN32)
  SET(SHAREDIR  "share/")
  SET(MENUDIR   "${SHAREDIR}/applications/")
  SET(ICONDIR   "${SHAREDIR}/pixmaps/")
ENDIF(NOT WIN32)

# portable edition
OPTION(PORTABLE "Enable to build in portable mode" OFF)
IF(PORTABLE_SPEEDCRUNCH)
  ADD_DEFINITIONS(-DSPEEDCRUNCH_PORTABLE)
ENDIF(PORTABLE_SPEEDCRUNCH)

# executable
INSTALL(TARGETS     ${PROGNAME}
        DESTINATION bin
)

IF(NOT WIN32)
# freedesktop menu
INSTALL(FILES       ../pkg/speedcrunch.desktop
        DESTINATION ${MENUDIR}
)

# icon
INSTALL(FILES       resources/speedcrunch.png
        DESTINATION ${ICONDIR}
)
ENDIF(NOT WIN32)

################################ UNINSTALL #####################################

CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
                IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

