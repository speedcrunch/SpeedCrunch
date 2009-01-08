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
              3rdparty/util

INCLUDEPATH += . math core 3rdparty/util gui
#INCLUDEPATH += .

# Input
HEADERS += core/constants.hxx \
           core/evaluator.hxx \
           core/functions.hxx \
           #core/settings.hxx \
           gui/aboutbox.hxx \
           gui/autohidelabel.hxx \
           gui/bookdock.hxx \
           gui/constantsdock.hxx \
           gui/deletevardlg.hxx \
           gui/resultdisplay.hxx \
           gui/editor.hxx \
           gui/functionsdock.hxx \
           gui/historydock.hxx \
           gui/insertfunctiondlg.hxx \
           gui/insertvardlg.hxx \
           gui/keypad.hxx \
           gui/tipwidget.hxx \
           gui/variablesdock.hxx \
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

SOURCES += 3rdparty/util/binreloc.c \
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
           gui/autohidelabel.cpp \
           gui/bookdock.cpp \
           gui/constantsdock.cpp \
           gui/deletevardlg.cpp \
           gui/resultdisplay.cpp \
           gui/editor.cpp \
           gui/functionsdock.cpp \
           gui/historydock.cpp \
           gui/insertfunctiondlg.cpp \
           gui/insertvardlg.cpp \
           gui/keypad.cpp \
           gui/tipwidget.cpp \
           gui/variablesdock.cpp \
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

RESOURCES += resources/speedcrunch.qrc
TRANSLATIONS += locale/ar.ts \
                locale/ca.ts \
                locale/cs.ts \
                locale/de.ts \
                locale/es.ts \
                locale/es_AR.ts \
                locale/eu.ts \
                locale/fi.ts \
                locale/fr.ts \
                locale/he.ts \
                locale/id.ts \
                locale/it.ts \
                locale/nb.ts \
                locale/nl.ts \
                locale/pl.ts \
                locale/pt.ts \
                locale/pt_BR.ts \
                locale/ro.ts \
                locale/ru.ts \
                locale/sv.ts \
                locale/tr.ts \
                locale/zh_CN.ts

win32:RC_FILE = resources/speedcrunch.rc
