TEMPLATE = app
TARGET = speedcrunch

# note: change this BEFORE making a release !
DEFINES += SPEEDCRUNCH_VERSION="trunk"

HEADERS += crunch.h
HEADERS += config.h
HEADERS += number.h
HEADERS += constants.h
HEADERS += historydock.h
HEADERS += functionsdock.h
HEADERS += variablesdock.h
HEADERS += constantsdock.h
HEADERS += hmath.h
HEADERS += evaluator.h
HEADERS += editor.h
HEADERS += result.h
HEADERS += functions.h
HEADERS += settings.h
HEADERS += insertvardlg.h
HEADERS += deletevardlg.h
HEADERS += insertfunctiondlg.h
HEADERS += configdlg.h
HEADERS += aboutbox.h
HEADERS += keypad.h
HEADERS += binreloc.h
HEADERS += autohidelabel.h
HEADERS += tipwidget.h

SOURCES += crunch.cpp
SOURCES += constants.cpp
SOURCES += historydock.cpp
SOURCES += functionsdock.cpp
SOURCES += variablesdock.cpp
SOURCES += constantsdock.cpp
SOURCES += hmath.cpp
SOURCES += number.c
SOURCES += evaluator.cpp
SOURCES += editor.cpp
SOURCES += result.cpp
SOURCES += functions.cpp
SOURCES += settings.cpp
SOURCES += insertvardlg.cpp
SOURCES += deletevardlg.cpp
SOURCES += insertfunctiondlg.cpp
SOURCES += configdlg.cpp
SOURCES += aboutbox.cpp
SOURCES += main.cpp
SOURCES += keypad.cpp
SOURCES += binreloc.c
SOURCES += autohidelabel.cpp
SOURCES += tipwidget.cpp


CONFIG += qt
CONFIG += thread
CONFIG += warn_on
CONFIG += release

unix:DEFINES += ENABLE_BINRELOC

RC_FILE = crunchico.rc

TRANSLATIONS += crunch_cs.ts
TRANSLATIONS += crunch_de.ts
TRANSLATIONS += crunch_fr.ts
TRANSLATIONS += crunch_id.ts
TRANSLATIONS += crunch_it.ts
TRANSLATIONS += crunch_nn.ts
TRANSLATIONS += crunch_pl_PL.ts
TRANSLATIONS += crunch_pt.ts
TRANSLATIONS += crunch_pt_BR.ts
TRANSLATIONS += crunch_ru.ts
TRANSLATIONS += crunch_es.ts

RESOURCES = crunch.qrc

include(conf.pri)

binaries.path = $${BINDIR}
binaries.files += speedcrunch
INSTALLS += binaries

translations.path = $${DATADIR}/crunch
translations.files += crunch_cs.qm
translations.files += crunch_de.qm
translations.files += crunch_fr.qm
translations.files += crunch_id.qm
translations.files += crunch_it.qm
translations.files += crunch_pt.qm
translations.files += crunch_pt_BT.qm
translations.files += crunch_ru.qm
translations.files += crunch_nn.qm
translations.files += crunch_pl_PL.qm
translations.files += crunch_es.qm
INSTALLS += translations

DISTFILES += README
DISTFILES += COPYING
DISTFILES += ChangeLog
DISTFILES += configure
DISTFILES += qcextra
DISTFILES += crunch.png
DISTFILES += clearinput.png
DISTFILES += LISEZMOI
DISTFILES += crunch_cs.qm
DISTFILES += crunch_de.qm
DISTFILES += crunch_fr.qm
DISTFILES += crunch_id.qm
DISTFILES += crunch_it.qm
DISTFILES += crunch_pt.qm
DISTFILES += crunch_pt_BR.qm
DISTFILES += crunch_ru.qm
DISTFILES += crunch_nn.qm
DISTFILES += crunch_pl_PL.qm
