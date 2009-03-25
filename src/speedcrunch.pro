!contains( QT_MAJOR_VERSION, 4 ) {
    error( unsupported Qt major version $$QT_MAJOR_VERSION found but 4 required )
}
!contains( QT_MINOR_VERSION, 3 ) {
    !contains( QT_MINOR_VERSION, 4 ) {
        !contains( QT_MINOR_VERSION, 5 ) {
            error( unsupported Qt minor version $$QT_MINOR_VERSION found but 3, 4 or 5 required )
        }
    }
}

DEFINES += SPEEDCRUNCH_VERSION=trunk

win32:DEFINES += _USE_MATH_DEFINES

TEMPLATE = app

TARGET = speedcrunch

DEPENDPATH += . \
              #bison \
              core \
              gui \
              locale \
              main \
              math \
              resources \
              #symboltables \
              #test \
              #variant \
              3rdparty/flickcharm
!macx {
    !win32 {
        DEPENDPATH += 3rdparty/binreloc
    }
}

INCLUDEPATH += . math core 3rdparty/flickcharm gui
!macx {
    !win32 {
        INCLUDEPATH += 3rdparty/binreloc
    }
}

QT += network

!macx {
    !win32 {
        DEFINES += ENABLE_BINRELOC
    }
}

# Input
HEADERS += 3rdparty/flickcharm/flickcharm.h \
           core/constants.hxx \
           core/evaluator.hxx \
           core/functions.hxx \
           #core/settings.hxx \
           gui/aboutbox.hxx \
           gui/application.hxx \
           gui/autohidelabel.hxx \
           gui/bookdock.hxx \
           gui/constantsdock.hxx \
           gui/constantswidget.hxx \
           gui/deletevardlg.hxx \
           gui/resultdisplay.hxx \
           gui/editor.hxx \
           gui/functionsdock.hxx \
           gui/functionswidget.hxx \
           gui/historydock.hxx \
           gui/historywidget.hxx \
           gui/insertfunctiondlg.hxx \
           gui/insertvardlg.hxx \
           gui/keypad.hxx \
           gui/tipwidget.hxx \
           gui/variablesdock.hxx \
           gui/variableswidget.hxx \
           gui/mainwindow.hxx
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
           #math/hmath.hxx \
           #math/number.h

#YACCSOURCES += bison/exprparser.y

SOURCES += 3rdparty/flickcharm/flickcharm.cpp \
           #variant/variantbase.cpp \
           #variant/variant.cpp \
           #variant/real.cpp \
           #variant/vstring.cpp \
           #variant/initvariant.cpp \
           #variant/formatbase.cpp \
           #variant/formatreal.cpp \
           core/constants.cpp \
           core/evaluator.cpp \
           core/functions.cpp \
           core/settings.cpp \
           gui/aboutbox.cpp \
           gui/application.cpp \
           gui/autohidelabel.cpp \
           gui/bookdock.cpp \
           gui/constantsdock.cpp \
           gui/constantswidget.cpp \
           gui/deletevardlg.cpp \
           gui/resultdisplay.cpp \
           gui/editor.cpp \
           gui/functionsdock.cpp \
           gui/functionswidget.cpp \
           gui/historydock.cpp \
           gui/historywidget.cpp \
           gui/insertfunctiondlg.cpp \
           gui/insertvardlg.cpp \
           gui/keypad.cpp \
           gui/tipwidget.cpp \
           gui/variablesdock.cpp \
           gui/variableswidget.cpp \
           gui/mainwindow.cpp \
           main/main.cpp \
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

!macx {
    !win32 {
        SOURCES += 3rdparty/binreloc/binreloc.c
    }
}

RESOURCES += resources/speedcrunch.qrc
TRANSLATIONS += locale/ar_JO.ts \
                locale/ca_ES.ts \
                locale/cs_CZ.ts \
                locale/de_DE.ts \
                locale/en_GB.ts \
                locale/en_US.ts \
                locale/es_AR.ts \
                locale/es_ES.ts \
                locale/eu_ES.ts \
                locale/fi_FI.ts \
                locale/fr_FR.ts \
                locale/he_IL.ts \
                locale/id_ID.ts \
                locale/it_IT.ts \
                locale/nb_NO.ts \
                locale/nl_NL.ts \
                locale/pl_PL.ts \
                locale/pt_BR.ts \
                locale/pt_PT.ts \
                locale/ro_RO.ts \
                locale/ru_RU.ts \
                locale/sv_SE.ts \
                locale/tr_TR.ts \
                locale/zh_CN.ts

win32:RC_FILE = resources/speedcrunch.rc

