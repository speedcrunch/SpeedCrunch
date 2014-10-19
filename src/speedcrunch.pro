equals(QT_MAJOR_VERSION, 5) {
    QT += widgets
}

equals(QT_MAJOR_VERSION, 4) : lessThan(QT_MINOR_VERSION, 8) {
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
        target.path = "/bin"
        menu.path = "/share/applications"
        icon.path = "/share/pixmaps"
        icon.files += resources/speedcrunch.png
        menu.files += ../pkg/speedcrunch.desktop
        INSTALLS += target icon menu
    }
}

macx {
    ICON = resources/speedcrunch.icns
    QMAKE_INFO_PLIST = ../pkg/Info.plist
    TARGET = SpeedCrunch
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
}

HEADERS += core/book.h \
           core/constants.h \
           core/evaluator.h \
           core/functions.h \
           core/manual.h \
           gui/aboutbox.h \
           gui/bitfieldwidget.h \
           gui/bookdock.h \
           gui/constantsdock.h \
           gui/constantswidget.h \
           gui/resultdisplay.h \
           gui/editor.h \
           gui/functionsdock.h \
           gui/functionswidget.h \
           gui/historydock.h \
           gui/historywidget.h \
           gui/tipwidget.h \
           gui/variablesdock.h \
           gui/variablelistwidget.h \
           gui/userfunctionsdock.h \
           gui/userfunctionlistwidget.h \
           gui/manualwindow.h \
           gui/mainwindow.h

SOURCES += main.cpp \
           core/book.cpp \
           core/constants.cpp \
           core/evaluator.cpp \
           core/functions.cpp \
           core/manual.cpp \
           core/numberformatter.cpp \
           core/pageserver.cpp \
           core/settings.cpp \
           gui/aboutbox.cpp \
           gui/bitfieldwidget.cpp \
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
           gui/userfunctionsdock.cpp \
           gui/userfunctionlistwidget.cpp \
           gui/mainwindow.cpp \
           gui/manualwindow.cpp \
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
                resources/locale/vi_VN.ts \
                resources/locale/zh_CN.ts
