// This file is part of the SpeedCrunch project
// Copyright (C) 2013 Helder Correia <helder.pereira.correia@gmail.com>
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

#include "core/numberformatter.h"

#include "core/settings.h"
#include "math/hmath.h"

QString NumberFormatter::format(const HNumber& number)
{
    Settings* settings = Settings::instance();
    const char format = number.format() != 0 ? number.format() : settings->resultFormat;
    char* str = HMath::format(number, format, settings->resultPrecision);
    QString result = QString::fromLatin1(str);
    free(str);
    if (settings->radixCharacter() != '.')
        result.replace('.', settings->radixCharacter());
    return result;
}