QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)
QT_VER_MIN = $$member(QT_VERSION, 1)

lessThan(QT_VER_MAJ, 4) | lessThan(QT_VER_MIN, 8) {
    error(Qt 4.8 or newer is required but version $$[QT_VERSION] was detected.)
}

DEFINES += SPEEDCRUNCH_VERSION=\\\"master\\\"
DEFINES += QT_USE_QSTRINGBUILDER
win32:DEFINES += _USE_MATH_DEFINES

TEMPLATE = app
TARGET = speedcrunch
QT += network

DEPENDPATH += . \
              core \
              gui \
              locale \
              math \
              #tests \
              resources

INCLUDEPATH += . math core gui

win32:RC_FILE = resources/speedcrunch.rc
win32-msvc*:LIBS += User32.lib
!macx {
    !win32 {
        DEPENDPATH += thirdparty
        INCLUDEPATH += thirdparty
        DEFINES += ENABLE_BINRELOC
        LIBS += -lX11
        SOURCES += thirdparty/binreloc.c
    }
}

macx:ICON = resources/speedcrunch.icns

HEADERS += core/book.h \
           core/constants.h \
           core/evaluator.h \
           core/functions.h \
           #core/numberformatter.h \
           #core/settings.h \
           gui/aboutbox.h \
           gui/application.h \
           gui/autohidelabel.h \
           gui/bookdock.h \
           gui/constantsdock.h \
           gui/constantswidget.h \
           gui/resultdisplay.h \
           gui/editor.h \
           gui/functionsdock.h \
           gui/functionswidget.h \
           gui/historydock.h \
           gui/historywidget.h \
           #gui/syntaxhighlighter.h \
           gui/tipwidget.h \
           gui/variablesdock.h \
           gui/variablelistwidget.h \
           gui/mainwindow.h
           #math/floatcommon.h \
           #math/floatconfig.h \
           #math/floatconst.h \
           #math/floatconvert.h \
           #math/floaterf.h \
           #math/floatexp.h \
           #math/floatgamma.h \
           #math/floathmath.h \
           #math/floatio.h \
           #math/floatipower.h \
           #math/floatlog.h \
           #math/floatlogic.h \
           #math/floatlong.h \
           #math/floatnum.h \
           #math/floatpower.h \
           #math/floatseries.h \
           #math/floattrig.h \
           #math/hmath.h \
           #math/number.h

SOURCES += main.cpp \
           core/book.cpp \
           core/constants.cpp \
           core/evaluator.cpp \
           core/functions.cpp \
           core/numberformatter.cpp \
           core/settings.cpp \
           gui/aboutbox.cpp \
           gui/application.cpp \
           gui/autohidelabel.cpp \
           gui/bookdock.cpp \
           gui/constantsdock.cpp \
           gui/constantswidget.cpp \
           gui/resultdisplay.cpp \
           gui/editor.cpp \
           gui/functionsdock.cpp \
           gui/functionswidget.cpp \
           gui/historydock.cpp \
           gui/historywidget.cpp \
           gui/syntaxhighlighter.cpp \
           gui/tipwidget.cpp \
           gui/variablesdock.cpp \
           gui/variablelistwidget.cpp \
           gui/mainwindow.cpp \
           math/floatcommon.c \
           math/floatconst.c \
           math/floatconvert.c \
           math/floaterf.c \
           math/floatexp.c \
           math/floatgamma.c \
           math/floathmath.c \
           math/floatio.c \
           math/floatipower.c \
           math/floatlog.c \
           math/floatlogic.c \
           math/floatlong.c \
           math/floatnum.c \
           math/floatpower.c \
           math/floatseries.c \
           math/floattrig.c \
           math/hmath.cpp \
           math/number.c

RESOURCES += resources/speedcrunch.qrc
TRANSLATIONS += resources/locale/ar_JO.ts \
                resources/locale/ca_ES.ts \
                resources/locale/cs_CZ.ts \
                resources/locale/de_DE.ts \
                resources/locale/en_GB.ts \
                resources/locale/en_US.ts \
                resources/locale/es_AR.ts \
                resources/locale/es_ES.ts \
                resources/locale/et_EE.ts \
                resources/locale/eu_ES.ts \
                resources/locale/fi_FI.ts \
                resources/locale/fr_FR.ts \
                resources/locale/he_IL.ts \
                resources/locale/hu_HU.ts \
                resources/locale/id_ID.ts \
                resources/locale/it_IT.ts \
                resources/locale/ja_JP.ts \
                resources/locale/ko_KR.ts \
                resources/locale/lv_LV.ts \
                resources/locale/nb_NO.ts \
                resources/locale/nl_NL.ts \
                resources/locale/pl_PL.ts \
                resources/locale/pt_BR.ts \
                resources/locale/pt_PT.ts \
                resources/locale/ro_RO.ts \
                resources/locale/ru_RU.ts \
                resources/locale/sv_SE.ts \
                resources/locale/tr_TR.ts \
                resources/locale/zh_CN.ts
