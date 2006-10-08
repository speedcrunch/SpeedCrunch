project(speedcrunch) 

cmake_minimum_required(VERSION 2.4.0)



# note: sort files alphabetically !

# sources
set(speedcrunch_SOURCES 
aboutbox.cpp
configdlg.cpp
crunch.cpp
crunchico.rc  # special
deletevardlg.cpp
editor.cpp
evaluator.cpp
functions.cpp
hmath.cpp
keypad.cpp
main.cpp
insertvardlg.cpp
insertfunctiondlg.cpp
number.c
result.cpp
settings.cpp
)

# headers
set(speedcrunch_HEADERS
aboutbox.h
configdlg.h
crunch.h
config.h
deletevardlg.h
editor.h
evaluator.h
functions.h
hmath.h
insertfunctiondlg.h
insertvardlg.h
keypad.h
number.h
result.h
settings.h
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
)

# we still need those Q3 classes
set( QT_USE_QT3SUPPORT TRUE )  

# enable warnings
ADD_DEFINITIONS( -Wall )

# setup for Qt4
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# build everything
qt4_add_RESOURCES( speedcrunch_RESOURCES_SOURCES ${speedcrunch_RESOURCES} )
QT4_WRAP_UI( speedcruch_FORMS_HEADERS ${speedcrunch_FORMS} )
QT4_WRAP_CPP( speedcrunch_HEADERS_MOC ${speedcrunch_HEADERS} )
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
ADD_EXECUTABLE( speedcrunch ${speedcrunch_SOURCES} ${speedcrunch_HEADERS_MOC} ${speedcrunch_RESOURCES_SOURCES} ${speedcruch_FORMS_HEADERS} )
target_link_libraries(speedcrunch ${QT_LIBRARIES})

# set(EXECUTABLE_OUTPUT_PATH ${speedcrunch_BINARY_DIR}/../bin)

# install executable and translation files
# note: it will install to CMAKE_INSTALL_PREFIX, which can be set e.g
#  cmake ../trunk --prefix=/usr
install(TARGETS speedcrunch DESTINATION bin)
install(FILES ${speedcrunch_TRANSLATIONS} DESTINATION share)
