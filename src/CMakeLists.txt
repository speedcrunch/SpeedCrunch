PROJECT(speedcrunch)

SET( speedcrunch_VERSION "trunk" )

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

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.4)
IF(COMMAND cmake_policy)
 CMAKE_POLICY(SET CMP0003 NEW)
 CMAKE_POLICY(SET CMP0005 OLD)   # remove this and the quoted arguments in
                                 # ADD_DEFINITONS() once CMake 2.6 is used
ENDIF(COMMAND cmake_policy)
SET(CMAKE_COLOR_MAKEFILE      ON)
SET(CMAKE_VERBOSE_MAKEFILE    OFF)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

ADD_DEFINITIONS("-DSPEEDCRUNCH_VERSION=\\\"${speedcrunch_VERSION}\\\"")

IF( NOT WIN32 )
IF( NOT APPLE )
  ADD_DEFINITIONS( -DENABLE_BINRELOC )
ENDIF( NOT APPLE )
ENDIF( NOT WIN32 )

# sources
INCLUDE(SourceFiles.cmake)

# resources
SET(speedcrunch_RESOURCES resources/speedcrunch.qrc)

IF( MINGW )
  # resource compilation for mingw
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o
                      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/resources/speedcrunchico.rc -o ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o )
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o)
ELSE(MINGW)
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} resources/speedcrunchico.rc)
ENDIF(MINGW)

# enable warnings
IF(CMAKE_COMPILER_IS_GNUCXX )
  ADD_DEFINITIONS( -Wall )
ENDIF(CMAKE_COMPILER_IS_GNUCXX )

# setup for Qt4
SET(QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# build everything
qt4_add_RESOURCES( speedcrunch_RESOURCES_SOURCES ${speedcrunch_RESOURCES} )
QT4_WRAP_UI( speedcrunch_FORMS_HEADERS ${speedcrunch_FORMS} )
QT4_WRAP_CPP( speedcrunch_HEADERS_MOC ${speedcrunch_HEADERS} )
ADD_EXECUTABLE( ${PROGNAME} WIN32 MACOSX_BUNDLE ${speedcrunch_SOURCES} ${speedcrunch_HEADERS_MOC} ${speedcrunch_RESOURCES_SOURCES} ${speedcruch_FORMS_HEADERS} )
IF( APPLE )
  SET( speedcrunch_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources )
  SET( speedcrunch_TRANSLATION_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/MacOS/locale )
  SET( speedcrunch_BOOKSDIR ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/MacOS/books )
  ADD_CUSTOM_COMMAND( TARGET ${PROGNAME} POST_BUILD
    COMMAND mkdir ARGS -p ${speedcrunch_RESOURCE_DIR}
    COMMAND mkdir ARGS -p ${speedcrunch_TRANSLATION_DIR}
    COMMAND mkdir ARGS -p ${speedcrunch_BOOKSDIR}
    COMMAND cp ARGS -f resources/${MACOSX_BUNDLE_ICON_FILE} ${speedcrunch_RESOURCE_DIR}
    COMMAND cp ARGS -f i18n/*.qm ${speedcrunch_TRANSLATION_DIR}
    COMMAND cp ARGS -Rf books/ ${speedcrunch_BOOKSDIR}
    COMMAND rm ARGS -f ${speedcrunch_BOOKSDIR}/genbooks.sh
    COMMAND rm ARGS -rf ${speedcrunch_BOOKSDIR}/i18n
    COMMAND for ARGS svnDir in `find ${speedcrunch_BOOKSDIR} -type d -name \".svn\"` \; do rm -rf $svnDir \; done
    COMMENT Setting up translations & resources )
ENDIF( APPLE )

ADD_CUSTOM_TARGET( confclean COMMAND rm -rf Makefile CMakeFiles/ CMakeCache.txt cmake_install.cmake DartTestfile.txt install_manifest.txt )

TARGET_LINK_LIBRARIES(${PROGNAME} ${QT_LIBRARIES})
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

#add_executable(testevaluator ${testevaluator_SOURCES})
#target_link_libraries(testevaluator ${QT_LIBRARIES})
#add_test(testevaluator ${EXECUTABLE_OUTPUT_PATH}/testevaluator)

ADD_EXECUTABLE(testfloatnum ${testfloatnum_SOURCES})
ADD_TEST(testfloatnum testfloatnum)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} 3rdparty/util core gui main math)


################################# INSTALL ######################################

# macros, definitions and options
IF(WIN32)
  SET(BOOKSDIR  books)
  SET(LOCALEDIR locale)
ELSE(WIN32)
  SET(SHAREDIR  "share/")
  SET(BOOKSDIR  "${SHAREDIR}/${PROGNAME}/books/")
  SET(LOCALEDIR "${SHAREDIR}/${PROGNAME}/locale/")
  SET(MENUDIR   "${SHAREDIR}/applications/")
  SET(ICONDIR   "${SHAREDIR}/pixmaps/")
ENDIF(WIN32)

ADD_DEFINITIONS("-DBOOKSDIR=\\\"${BOOKSDIR}\\\"")

# portable edition
OPTION(PORTABLE "Enable to build in portable mode" OFF)
IF(PORTABLE_SPEEDCRUNCH)
  ADD_DEFINITIONS(-DSPEEDCRUNCH_PORTABLE)
ENDIF(PORTABLE_SPEEDCRUNCH)

# executable
INSTALL(TARGETS     ${PROGNAME}
        DESTINATION bin
)

# translations (.qm files)
INSTALL(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/locale/
        DESTINATION ${LOCALEDIR}
        PATTERN     "locale/*.qm"
        PATTERN     "locale/*.ts" EXCLUDE
)

# books
INSTALL(DIRECTORY   books/
        DESTINATION ${BOOKSDIR}
        PATTERN     "i18n" EXCLUDE
        PATTERN     "books/images/*.mml" EXCLUDE
        PATTERN     "books/images/*.sh" EXCLUDE
        PATTERN     "books/images/*.svgz" EXCLUDE
        PATTERN     "books/*.sh" EXCLUDE
)

IF(NOT WIN32)
# freedesktop menu
INSTALL(FILES       speedcrunch.desktop
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

