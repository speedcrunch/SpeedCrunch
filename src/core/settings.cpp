// This file is part of the SpeedCrunch project
// Copyright (C) 2004, 2005, 2007, 2008 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2009 Helder Correia <helder.pereira.correia@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "core/settings.h"

#include "thirdparty/binreloc.h"
#include "math/floatconfig.h"

#include <QDir>
#include <QLocale>
#include <QSettings>
#include <QApplication>
#include <QFont>

static Settings* s_settingsInstance = 0;
static char s_radixCharacter = 0;

static void s_deleteSettings()
{
    delete s_settingsInstance;
}

static QSettings* createQSettings(const QString& key);

Settings* Settings::instance()
{
    if (!s_settingsInstance) {
        s_settingsInstance = new Settings;
        s_settingsInstance->load();
        qAddPostRoutine(s_deleteSettings);
    }

    return s_settingsInstance;
}

Settings::Settings()
{
}

void Settings::load()
{
    const char* KEY = "SpeedCrunch";

    QSettings* settings = createQSettings(KEY);
    if (!settings)
        return;

    QString key = QString::fromLatin1(KEY) + QLatin1String("/General/");

    // Angle mode special case.
    QString angleUnitStr;
    angleUnitStr = settings->value(key + QLatin1String("AngleMode"), "r").toString();
    if (angleUnitStr != QLatin1String("r") && angleUnitStr != QLatin1String("d"))
        angleUnit = 'r';
    else
        angleUnit = angleUnitStr.at(0).toLatin1();

    // Radix character special case.
    QString radixCharStr;
    radixCharStr = settings->value(key + QLatin1String("RadixCharacter"), 0).toString();
    setRadixCharacter(radixCharStr.at(0).toLatin1());

    autoAns = settings->value(key + QLatin1String("AutoAns"), true).toBool();
    autoCalc = settings->value(key + QLatin1String("AutoCalc"), true).toBool();
    autoCompletion = settings->value(key + QLatin1String("AutoCompletion"), true).toBool();
    historySave = settings->value(key + QLatin1String("HistorySave"), true).toBool();
    leaveLastExpression = settings->value(key + QLatin1String("LeaveLastExpression"), false).toBool();
    language = settings->value(key + QLatin1String("Language"), "C").toString();
    variableSave = settings->value(key + QLatin1String("VariableSave"), true).toBool();
    userFunctionSave = settings->value(key + QLatin1String("UserFunctionSave"), true).toBool();
    syntaxHighlighting = settings->value(key + QLatin1String("SyntaxHighlighting"), true).toBool();
    systemTrayIconVisible = settings->value(key + QLatin1String("SystemTrayIconVisible"), false).toBool();
    autoResultToClipboard = settings->value(key + QLatin1String("AutoResultToClipboard"), false).toBool();
    windowPositionSave = settings->value(key + QLatin1String("WindowPositionSave"), true).toBool();

    digitGrouping = settings->value(key + QLatin1String("DigitGrouping"), 0).toInt();
    digitGrouping = std::min(3, std::max(0, digitGrouping));

    key = KEY + QLatin1String("/Format/");

    // Format special case.
    QString format;
    format = settings->value(key + QLatin1String("Type"), 'f').toString();
    if (format != "g" && format != "f" && format != "e" && format != "n"&& format != "h" && format != "o" && format != "b")
        resultFormat = 'f';
    else
        resultFormat = format.at(0).toLatin1();

    resultPrecision = settings->value(key + QLatin1String("Precision"), -1).toInt();

    if (resultPrecision > DECPRECISION)
        resultPrecision = DECPRECISION;

    key = KEY + QLatin1String("/Layout/");
    windowOnfullScreen = settings->value(key + QLatin1String("WindowOnFullScreen"), false).toBool();
    historyDockVisible = settings->value(key + QLatin1String("HistoryDockVisible"), false).toBool();
    menuBarVisible = settings->value(key + QLatin1String("MenuBarVisible"), true).toBool();
    statusBarVisible = settings->value(key + QLatin1String("StatusBarVisible"), false).toBool();
    functionsDockVisible = settings->value(key + QLatin1String("FunctionsDockVisible"), false).toBool();
    variablesDockVisible = settings->value(key + QLatin1String("VariablesDockVisible"), false).toBool();
    userFunctionsDockVisible = settings->value(key + QLatin1String("UserFunctionsDockVisible"), false).toBool();
    formulaBookDockVisible = settings->value(key + QLatin1String("FormulaBookDockVisible"), false).toBool();
    constantsDockVisible = settings->value(key + QLatin1String("ConstantsDockVisible"), false).toBool();
    windowAlwaysOnTop = settings->value(key + QLatin1String("WindowAlwaysOnTop"), false).toBool();
    bitfieldVisible = settings->value(key + QLatin1String("BitfieldVisible"), false).toBool();

    windowPosition = settings->value(key + QLatin1String("WindowPosition"), QPoint()).toPoint();
    windowSize = settings->value(key + QLatin1String("WindowSize"), QSize(640, 480)).toSize();
    windowState = settings->value(key + QLatin1String("State")).toByteArray();
    maximized = settings->value(key + QLatin1String("Maximized"), false).toBool();

    key = KEY + QLatin1String("/Display/");
    displayFont = settings->value(key + QLatin1String("DisplayFont"), QFont().toString()).toString();
    colorScheme = settings->value(key + QLatin1String("ColorScheme"), 0).toInt();

    // Load history.
    key = KEY + QLatin1String("/History/");
    history.clear();
    int count = settings->value(key + QLatin1String("Count"), 0).toInt();
    for (int i = 0; i < count; ++i) {
        QString keyname = QString(key + QLatin1String("Expression%1")).arg(i);
        QString str = settings->value(keyname).toString();
        if (!str.isEmpty()) {
            QString expr;
            for (int c = 0; c < str.length(); ++c)
                if (str.at(c) >= 32)
                    expr.append(str.at(c));
            history.append(expr);
        }
    }

    // Load results.
    historyResults.clear();
    for (int i = 0; i < count; ++i) {
        QString keyname = QString(key + QLatin1String("Expression%1Result")).arg(i);
        QVariant value = settings->value(keyname);
        if (value.isValid())
            historyResults.append(value.toString());
    }
    if (history.count() != historyResults.count()) {
        // Avoid application crash because of new features with old settings files.
        history.clear();
        historyResults.clear();
    }

    // Load variables.
    key = KEY + QLatin1String("/Variables/");
    variables.clear();
    settings->beginGroup(key);
    QStringList names = settings->childKeys();
    settings->endGroup();
    for (int k = 0; k < names.count(); ++k) {
        QString name = names.at(k);
        QString keyname = key + name;
        QString value = settings->value(keyname).toString();
        // Treat upper case escape code.
        for (int c = 0; c < name.length(); ++c) {
            if (name.at(c) == '_') {
                name.remove(c, 1);
                if (name.at(c) != '_')
                    name[c] = name.at(c).toUpper();
            }
        }
        // Load.
        if (!value.isEmpty())
            variables.append(QString::fromLatin1("%1=%2").arg(name).arg(value));
    }

    // Load user functions.
    key = KEY + QLatin1String("/UserFunctions/");
    userFunctions.clear();
    settings->beginGroup(key);
    names = settings->childKeys();
    settings->endGroup();
    for (int k = 0; k < names.count(); ++k) {
        QString name = names.at(k);
        QString keyname = key + name;
        QStringList value = settings->value(keyname).toStringList();
        // Treat upper case escape code.
        for (int c = 0; c < name.length(); ++c) {
            if (name.at(c) == '_') {
                name.remove(c, 1);
                if (name.at(c) != '_')
                    name[c] = name.at(c).toUpper();
            }
        }
        // Load.
        if (!value.isEmpty()) {
            value.prepend(name);
            userFunctions.append(value);
        }
    }

    delete settings;
}

void Settings::save()
{
    const QString KEY = QString::fromLatin1("SpeedCrunch");

    QSettings* settings = createQSettings(KEY);
    if (!settings)
        return;

    int k, i;
    QString key = KEY + QLatin1String("/General/");

    settings->setValue(key + QLatin1String("HistorySave"), historySave);
    settings->setValue(key + QLatin1String("LeaveLastExpression"), leaveLastExpression);
    settings->setValue(key + QLatin1String("VariableSave"), variableSave);
    settings->setValue(key + QLatin1String("UserFunctionSave"), userFunctionSave);
    settings->setValue(key + QLatin1String("AutoCompletion"), autoCompletion);
    settings->setValue(key + QLatin1String("AutoAns"), autoAns);
    settings->setValue(key + QLatin1String("AutoCalc"), autoCalc);
    settings->setValue(key + QLatin1String("SystemTrayIconVisible"), systemTrayIconVisible);
    settings->setValue(key + QLatin1String("SyntaxHighlighting"), syntaxHighlighting);
    settings->setValue(key + QLatin1String("DigitGrouping"), digitGrouping);
    settings->setValue(key + QLatin1String("AutoResultToClipboard"), autoResultToClipboard);
    settings->setValue(key + QLatin1String("Language"), language);
    settings->setValue(key + QLatin1String("WindowPositionSave"), windowPositionSave);

    settings->setValue(key + QLatin1String("AngleMode"), QString(QChar(angleUnit)));

    char c = 'C';
    if (s_radixCharacter != 0)
        c = s_radixCharacter;
    settings->setValue(key + QLatin1String("RadixCharacter"), QString(QChar(c)));

    key = KEY + QLatin1String("/Format/");

    settings->setValue(key + QLatin1String("Type"), QString(QChar(resultFormat)));
    settings->setValue(key + QLatin1String("Precision"), resultPrecision);

    key = KEY + QLatin1String("/Layout/");

    settings->setValue(key + QLatin1String("FormulaBookDockVisible"), formulaBookDockVisible);
    settings->setValue(key + QLatin1String("ConstantsDockVisible"), constantsDockVisible);
    settings->setValue(key + QLatin1String("FunctionsDockVisible"), functionsDockVisible);
    settings->setValue(key + QLatin1String("HistoryDockVisible"), historyDockVisible);
    settings->setValue(key + QLatin1String("WindowOnFullScreen"), windowOnfullScreen);
    settings->setValue(key + QLatin1String("MenuBarVisible"), menuBarVisible);
    settings->setValue(key + QLatin1String("StatusBarVisible"), statusBarVisible);
    settings->setValue(key + QLatin1String("VariablesDockVisible"), variablesDockVisible);
    settings->setValue(key + QLatin1String("UserFunctionsDockVisible"), userFunctionsDockVisible);
    settings->setValue(key + QLatin1String("WindowPosition"), windowPosition);
    settings->setValue(key + QLatin1String("WindowSize"), windowSize);
    settings->setValue(key + QLatin1String("WindowAlwaysOnTop"), windowAlwaysOnTop);
    settings->setValue(key + QLatin1String("State"), windowState);
    settings->setValue(key + QLatin1String("Maximized"), maximized);
    settings->setValue(key + QLatin1String("BitfieldVisible"), bitfieldVisible);

    key = KEY + QLatin1String("/Display/");

    settings->setValue(key + QLatin1String("DisplayFont"), displayFont);
    settings->setValue(key + QLatin1String("ColorScheme"), colorScheme);

    // Save history.
    if (historySave) {
        key = KEY + QLatin1String("/History/");
        QStringList realHistory = history;
        QStringList realHistoryResults = historyResults;

        if (history.count() > 100) {
            realHistory.clear();
            realHistoryResults.clear();
            unsigned start = history.count() - 100;
            for (int j = start; j < history.count(); ++j) {
                realHistory.append(history.at(j));
                realHistoryResults.append(historyResults.at(j));
            }
        }

        settings->beginGroup(key);
        QStringList hkeys = settings->childKeys();
        settings->endGroup();

        for (k = 0; k < hkeys.count(); k++) {
            settings->remove(key + "Expression" + QString::number(k));
            settings->remove(key + "Expression" + QString::number(k) + "Result");
        }

        settings->setValue(key + QLatin1String("/Count/"), (int)history.count());
        for (i = 0; i < realHistory.count(); i++) {
            settings->setValue((key + "Expression" + QString::number(i)), realHistory.at(i));
            settings->setValue((key + "Expression" + QString::number(i) + "Result"), realHistoryResults.at(i));
        }
    }

    // Save variables.
    if (variableSave) {
        key = KEY + QLatin1String("/Variables/");
        settings->beginGroup(key);
        QStringList vkeys = settings->childKeys();
        settings->endGroup();

        for (k = 0; k < vkeys.count(); ++k)
            settings->remove(key + vkeys.at(k));

        for (i = 0; i < variables.count(); ++i) {
            QStringList s = variables[i].split('=');
            if (s.count() == 2) {
                QString name = "";
                QString value = s.at(1);
                int length = s.at(0).length();
                for (int c = 0; c < length; ++c) {
                    if (s.at(0).at(c).isUpper() || s.at(0).at(c) == '_') {
                        name += '_';
                        name += s.at(0).at(c).toLower();
                    } else
                        name += s.at(0).at(c);
                }
                settings->setValue(key + name, value);
            }
        }
    }

    // Save user functions.
    if (userFunctionSave) {
        key = KEY + QLatin1String("/UserFunctions/");
        settings->beginGroup(key);
        QStringList vkeys = settings->childKeys();
        settings->endGroup();

        for (k = 0; k < vkeys.count(); ++k)
            settings->remove(key + vkeys.at(k));

        for (i = 0; i < userFunctions.count(); ++i) {
            QStringList s = userFunctions.at(i);
            if (s.count() >= 2) {
                QString name = "";
                QVariant value(s.mid(1));
                int length = s.at(0).length();
                for (int c = 0; c < length; ++c) {
                    if (s.at(0).at(c).isUpper() || s.at(0).at(c) == '_') {
                        name += '_';
                        name += s.at(0).at(c).toLower();
                    } else
                        name += s.at(0).at(c);
                }
                settings->setValue(key + name, value);
            }
        }
    }

    delete settings;
}

char Settings::radixCharacter() const
{
    return s_radixCharacter == 0 ? QLocale().decimalPoint().toLatin1() : s_radixCharacter;
}

bool Settings::isRadixCharacterAuto() const
{
    return s_radixCharacter == 0;
}

void Settings::setRadixCharacter(char c)
{
    s_radixCharacter = (c != ',' && c != '.') ? 0 : c;
};

QSettings* createQSettings(const QString& KEY)
{
    QSettings* settings = 0;

#ifdef SPEEDCRUNCH_PORTABLE
#if defined(Q_WS_WIN)
    // Portable Windows version: settings are from INI file in same directory.
    QString appPath = QApplication::applicationFilePath();
    int ii = appPath.lastIndexOf('/');
    if (ii > 0)
        appPath.remove(ii, appPath.length());
    QString iniFile = appPath + '/' + KEY + ".ini";
    settings = new QSettings(iniFile, QSettings::IniFormat);
#elif defined(Q_WS_X11) || defined (Q_WS_QWS)
    // Portable X11 version: settings are from INI file in the same directory.
    BrInitError error;
    if (br_init(& error) == 0 && error != BR_INIT_ERROR_DISABLED) {
        qDebug("Warning: BinReloc failed to initialize (error code %d)", error);
        qDebug("Will fallback to hardcoded default path.");
    }

    const char* prefix = br_find_prefix(0);
    QString iniFile = QString(prefix) + '/' + KEY + QLatin1String(".ini");
    free(prefix);
    settings = new QSettings(iniFile, QSettings::IniFormat);
#endif // Q_WS_WIN / Q_WS_X11 || Q_WS_QWS
#else // SPEEDCRUNCH_PORTABLE
    settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, KEY, KEY);
#endif // SPEEDCRUNCH_PORTABLE

    return settings;
}
