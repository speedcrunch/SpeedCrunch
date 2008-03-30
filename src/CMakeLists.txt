PROJECT(speedcrunch)

IF( APPLE )
  SET( PROGNAME SpeedCrunch )
  SET( MACOSX_BUNDLE_ICON_FILE SpeedCrunch.icns )
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING trunk )
  SET( MACOSX_BUNDLE_VERSION trunk )
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version trunk )
  SET( CMAKE_OSX_ARCHITECTURES ppc;i386 ) #Comment out if not universal binary
ELSE( APPLE )
  SET( PROGNAME speedcrunch )
ENDIF( APPLE )

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.4)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

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
SET(speedcrunch_RESOURCES resources/speedcrunch.qrc)

# translations
INCLUDE(Translations.cmake)

IF( MINGW )
  # resource compilation for mingw
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o
                      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/resources/speedcrunchico.rc -o ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o )
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/speedcrunchico.o)
  SET(LINK_FLAGS -Wl,-subsystem,windows)
ELSE(MINGW)
  SET(speedcrunch_SOURCES ${speedcrunch_SOURCES} resources/speedcrunchico.rc)
ENDIF(MINGW)

# enable warnings
IF(CMAKE_COMPILER_IS_GNUCXX )
  ADD_DEFINITIONS( -g -Wall )
ENDIF(CMAKE_COMPILER_IS_GNUCXX )

# setup for Qt4
SET(QT_USE_QTXML TRUE)
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
  #ADD_DEPENDENCIES( ${PROGNAME} SpeedCrunch_RESOURCE_DIR )
ELSE( APPLE )
  ADD_EXECUTABLE( ${PROGNAME} ${speedcrunch_SOURCES} ${speedcrunch_HEADERS_MOC} ${speedcrunch_RESOURCES_SOURCES} ${speedcrunch_FORMS_HEADERS} )
ENDIF( APPLE )

ADD_CUSTOM_TARGET( confclean COMMAND rm -rf Makefile CMakeFiles/ CMakeCache.txt cmake_install.cmake DartTestfile.txt install_manifest.txt )

TARGET_LINK_LIBRARIES(${PROGNAME} ${QT_LIBRARIES} ${LINK_FLAGS})

# set(EXECUTABLE_OUTPUT_PATH ${speedcrunch_BINARY_DIR}/../bin)

# install executable and translation files
# note: it will install to CMAKE_INSTALL_PREFIX, which can be set e.g
#  cmake ../trunk  -DCMAKE_INSTALL_PREFIX=/usr
INSTALL(TARGETS ${PROGNAME} DESTINATION bin)
INSTALL(FILES ${speedcrunch_TRANSLATIONS} DESTINATION share/speedcrunch/locale)

# required for regression tests
ENABLE_TESTING()

ADD_EXECUTABLE(testhmath ${testhmath_SOURCES})
ADD_TEST(testhmath ${EXECUTABLE_OUTPUT_PATH}/testhmath)

#add_executable(testevaluator ${testevaluator_SOURCES})
#target_link_libraries(testevaluator ${QT_LIBRARIES})
#add_test(testevaluator ${EXECUTABLE_OUTPUT_PATH}/testevaluator)

ADD_EXECUTABLE(testfloatnum ${testfloatnum_SOURCES})
ADD_TEST(testfloatnum ${EXECUTABLE_OUTPUT_PATH}/testfloatnum)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} 3rdparty/util base gui main math)

# books
SET(BOOKSDIR "${CMAKE_INSTALL_PREFIX}/share/${PROGNAME}/books/")
ADD_DEFINITIONS("-DBOOKSDIR=\\\"${BOOKSDIR}\\\"")

INSTALL(FILES       books/books.css
        DESTINATION ${BOOKSDIR}
)

INSTALL(DIRECTORY   books/images books/cs books/de books/es books/es_AR books/en books/fi books/fr books/he books/id books/it books/nl books/no books/pt books/pt_BR books/pl books/ro books/ru books/sv books/tr
        DESTINATION "${BOOKSDIR}"
        PATTERN     ".svn" EXCLUDE
)

# freedesktop file
INSTALL(FILES       speedcrunch.desktop
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications/"
)

INSTALL(FILES       resources/speedcrunch.png
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps/"
)
