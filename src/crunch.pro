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

RC_FILE = resources/crunchico.rc

TRANSLATIONS += ../i18n/crunch_cs.ts
TRANSLATIONS += ../i18n/crunch_de.ts
TRANSLATIONS += ../i18n/crunch_es.ts
TRANSLATIONS += ../i18n/crunch_es_AR.ts
TRANSLATIONS += ../i18n/crunch_fi.ts
TRANSLATIONS += ../i18n/crunch_fr.ts
TRANSLATIONS += ../i18n/crunch_he.ts
TRANSLATIONS += ../i18n/crunch_id.ts
TRANSLATIONS += ../i18n/crunch_it.ts
TRANSLATIONS += ../i18n/crunch_nl.ts
TRANSLATIONS += ../i18n/crunch_no.ts
TRANSLATIONS += ../i18n/crunch_pl.ts
TRANSLATIONS += ../i18n/crunch_pt.ts
TRANSLATIONS += ../i18n/crunch_pt_BR.ts
TRANSLATIONS += ../i18n/crunch_ro.ts
TRANSLATIONS += ../i18n/crunch_ru.ts
TRANSLATIONS += ../i18n/crunch_sv.ts
TRANSLATIONS += ../i18n/crunch_sl.ts
TRANSLATIONS += ../i18n/crunch_tr.ts

RESOURCES = resources/crunch.qrc

include(conf.pri)

binaries.path = $${BINDIR}
binaries.files += speedcrunch
INSTALLS += binaries

translations.path = $${DATADIR}/crunch
translations.files += ../i18n/crunch_cs.qm
translations.files += ../i18n/crunch_de.qm
translations.files += ../i18n/crunch_es.qm
translations.files += ../i18n/crunch_es_AR.qm
translations.files += ../i18n/crunch_fi.qm
translations.files += ../i18n/crunch_fr.qm
translations.files += ../i18n/crunch_he.qm
translations.files += ../i18n/crunch_id.qm
translations.files += ../i18n/crunch_it.qm
translations.files += ../i18n/crunch_nl.qm
translations.files += ../i18n/crunch_no.qm
translations.files += ../i18n/crunch_pl.qm
translations.files += ../i18n/crunch_pt.qm
translations.files += ../i18n/crunch_pt_BR.qm
translations.files += ../i18n/crunch_ro.qm
translations.files += ../i18n/crunch_ru.qm
translations.files += ../i18n/crunch_sv.qm
translations.files += ../i18n/crunch_sl.qm
translations.files += ../i18n/crunch_tr.qm
INSTALLS += translations

DISTFILES += README
DISTFILES += COPYING
DISTFILES += ChangeLog
DISTFILES += configure
DISTFILES += qcextra
DISTFILES += crunch.png
DISTFILES += clearinput.png
DISTFILES += LISEZMOI
DISTFILES += ../i18n/crunch_cs.qm
DISTFILES += ../i18n/crunch_de.qm
DISTFILES += ../i18n/crunch_es.qm
DISTFILES += ../i18n/crunch_es_AR.qm
DISTFILES += ../i18n/crunch_fi.qm
DISTFILES += ../i18n/crunch_fr.qm
DISTFILES += ../i18n/crunch_he.qm
DISTFILES += ../i18n/crunch_id.qm
DISTFILES += ../i18n/crunch_it.qm
DISTFILES += ../i18n/crunch_nl.qm
DISTFILES += ../i18n/crunch_no.qm
DISTFILES += ../i18n/crunch_pl.qm
DISTFILES += ../i18n/crunch_pt.qm
DISTFILES += ../i18n/crunch_pt_BR.qm
DISTFILES += ../i18n/crunch_ro.qm
DISTFILES += ../i18n/crunch_ru.qm
DISTFILES += ../i18n/crunch_sv.qm
DISTFILES += ../i18n/crunch_sl.qm
DISTFILES += ../i18n/crunch_tr.qm
