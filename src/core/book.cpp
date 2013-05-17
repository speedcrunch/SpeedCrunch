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

#include "core/book.h"

#define STYLE \
    "<style>" \
        "body {" \
            "background-color: #ffffff;" \
            "color: #000000;" \
        "}" \
        \
        "h2 {" \
            "font-family: \"Times New Roman\", Verdana, Arial, Helvetica, sans-serif;" \
        "}" \
        \
        ".variable {" \
            "font-weight: bold;" \
            "font-size: 10pt;" \
        "}" \
        \
        ".unit {" \
            "font-size: 8pt;" \
            "font-style: italic;" \
        "}" \
        \
        ".caption {" \
            "font-style: italic;" \
            "font-size: 10pt;" \
        \
        "}" \
        \
        "a:link {" \
            "text-decoration: none;" \
        "}" \
        \
        ".page-link a:link {" \
            "color: #555;" \
            "font-family: Verdana, Arial, Helvetica, sans-serif;" \
        "}" \
        \
        ".formula a:link {" \
            "color: SteelBlue;" \
            "font-size: 15pt;" \
            "font-family: \"Times New Roman\", Verdana, Arial, Helvetica, sans-serif;" \
        "}" \
    "</style>"

#define BEGIN QString("<html><head>"STYLE"</head><body>")+
#define SECTION(s) "<h3>"+(s)+"</h3>"
#define BR "<br />"
#define INDEX_LINK "<span class=\"page-link\"><a href=\"index\">"+Book::tr("Index")+"</a></span>"
#define LINK(link,text) "<span class=\"page-link\"><a href=\""#link"\">"+text+"</a></span>"
#define TITLE(s) "<h1>"+(s)+"</h1><div>"
#define SUBTITLE(s) "<h2>"#s+"</h2><div>"
#define FORMULA(link,text) "<span class=\"formula\"><a href=\"formula:"#link"\">"#text"</a></span><br />"
#define FORMULA_UNIT(link,text,unit) "<span class=\"formula\"><a href=\"formula:"#link"\">"#text"</a></span> <span class=\"unit\">"#unit"</span><br />"
#define TABLE "<table><tr>"
#define ROW "</tr><tr>"
#define _TABLE "</div></tr></table>"
#define VARIABLE(s) "<td class=\"variable\">"#s"</td>"
#define CAPTION(s) "<td class=\"caption\">"+(s)+"</td>"
#define END "</body></html>"

static QString makeIndexPage()
{
    return
        BEGIN
        TITLE(Book::tr("Index"))
        SECTION(Book::tr("Algebra"))
        LINK(algebra/quadratic-equation, Book::tr("Quadratic Equation")) BR
        LINK(algebra/log-base-conversion, Book::tr("Logarithmic Base Conversion")) BR
        SECTION(Book::tr("Geometry"))
        LINK(geometry/circle, Book::tr("Circle")) BR
        LINK(geometry/sector, Book::tr("Sector")) BR
        LINK(geometry/sphere, Book::tr("Sphere")) BR
        LINK(geometry/cube, Book::tr("Cube")) BR
        LINK(geometry/cone, Book::tr("Cone")) BR
        LINK(geometry/cylinder, Book::tr("Cylinder")) BR
        SECTION(Book::tr("Unit Conversions"))
        LINK(units/temperature, Book::tr("Temperature")) BR
        SECTION(Book::tr("Electronics"))
        LINK(electronics/ohmslaw, Book::tr("Ohm's Law")) BR
        LINK(electronics/power, Book::tr("Power")) BR
        LINK(electronics/reactance, Book::tr("Reactance")) BR
        LINK(electronics/resonance, Book::tr("Resonance")) BR
        SECTION(Book::tr("Radio Frequency"))
        LINK(rf/antennas, Book::tr("Antennas")) BR
        LINK(rf/impedance, Book::tr("Characteristic Impedance (coax)")) BR
        LINK(rf/propagation, Book::tr("Velocity of Propagation (coax)")) BR
        LINK(rf/swr, Book::tr("Standing Wave Ratio & Return Loss")) BR
        LINK(rf/wavelength, Book::tr("Free Space Wavelength")) BR
        END;
}

static QString makeAlgebraQuadraticEquationPage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Quadratic Equation"))
        SUBTITLE(a&sdot;x<sup>2</sup> + b&sdot;x + c = 0)
        FORMULA(x1 = (-b + sqrt(b%5e2 - 4*a*c))/(2*a), x<sub>1</sub> = (-b + &radic;(b<sup>2</sup> - 4&sdot;a&sdot;c)) / (2&sdot;a))
        FORMULA(x2 = (-b - sqrt(b%5e2 - 4*a*c))/(2*a), x<sub>2</sub> = (-b - &radic;(b<sup>2</sup> - 4&sdot;a&sdot;c)) / (2&sdot;a))
        END;
}

static QString makeAlgebraLogBaseConversionPage() {
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Logarithmic Base Conversion"))
        FORMULA(y = log(x) / log(a), log<sub>a</sub>x = log(x) / log(a))
        END;
}

static QString makeElectronicsOhmsLawPage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Ohm's Law"))
        FORMULA_UNIT(R = V / I, R = V / I, &Omega;)
        FORMULA_UNIT(V = I * R, V = I&sdot;R, V)
        FORMULA_UNIT(I = V / R, I = V / R, A)
        TABLE
        VARIABLE(R) CAPTION(Book::tr("resistance")) ROW
        VARIABLE(V) CAPTION(Book::tr("voltage")) ROW
        VARIABLE(I) CAPTION(Book::tr("current")) ROW
        _TABLE
        END;
}

static QString makeElectronicsPowerPage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Power"))
        FORMULA_UNIT(P = I * V, P = I&sdot;V, W)
        FORMULA_UNIT(P = I%5e2 * R, P = I<sup>2</sup>&sdot;R, W)
        FORMULA_UNIT(P = V%5e2 / R, P = V<sup>2</sup> / R, W)
        FORMULA_UNIT(I = P / V, I = P / V, A)
        FORMULA_UNIT(I = sqrt(P / R), I = &radic;(P / R), A)
        FORMULA_UNIT(V = P / I, V = P / I, V)
        FORMULA_UNIT(V = sqrt(P * R), V = &radic;(P&sdot;R), V)
        FORMULA_UNIT(R = V%5e2 / P, R = V<sup>2</sup> / P, &Omega;)
        FORMULA_UNIT(R = P / I%5e2, R = P / I<sup>2</sup>, &Omega;)
        TABLE
        VARIABLE(P) CAPTION(Book::tr("power")) ROW
        VARIABLE(I) CAPTION(Book::tr("current")) ROW
        VARIABLE(V) CAPTION(Book::tr("voltage")) ROW
        VARIABLE(R) CAPTION(Book::tr("resistance")) ROW
        _TABLE
        END;
}

static QString makeElectronicsReactancePage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Reactance"))
        FORMULA_UNIT(Xl = 2 * pi * f * L, X<sub>L</sub> = 2&sdot;&pi;&sdot;f&sdot;L, &Omega;)
        FORMULA_UNIT(Xc = 1 / (2 * pi * f * C), X<sub>C</sub> = 1 / (2&sdot;&pi;&sdot;f&sdot;C), &Omega;)
        FORMULA_UNIT(L = Xl / (2 * pi * f), L = X<sub>L</sub> / (2&sdot;&pi;&sdot;f), H)
        FORMULA_UNIT(C = 1 / (2 * pi * f * Xc), C = 1 / (2&sdot;&pi;&sdot;f&sdot;X<sub>C</sub>), F)
        FORMULA_UNIT(f = Xl / (2 * pi * L), f = X<sub>L</sub> / (2&sdot;&pi;&sdot;L), Hz)
        FORMULA_UNIT(f = 1 / (Xc * 2 * pi * C), f = 1 / (X<sub>C</sub>&sdot;2&sdot;&pi;&sdot;C), Hz)
        TABLE
        VARIABLE(X<sub>L</sub>) CAPTION(Book::tr("inductive reactance")) ROW
        VARIABLE(X<sub>C</sub>) CAPTION(Book::tr("capacitive reactance")) ROW
        VARIABLE(L) CAPTION(Book::tr("inductance")) ROW
        VARIABLE(C) CAPTION(Book::tr("capacitance")) ROW
        VARIABLE(f) CAPTION(Book::tr("frequency")) ROW
        _TABLE
        END;
}

static QString makeElectronicsResonancePage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Resonance"))
        FORMULA_UNIT(f = 1 / (2 * pi * sqrt(L * C)), f = 1 / (2&sdot;&pi;&sdot;&radic;(L&sdot;C)), Hz)
        FORMULA_UNIT(L = 1 / (4 * pi%5e2 * f%5e2 * C), L = 1 / (4&sdot;&pi;<sup>2</sup>&sdot;f<sup>2</sup>&sdot;C), H)
        FORMULA_UNIT(C = 1 / (4 * pi%5e2 * f%5e2 * L), C = 1 / (4&sdot;&pi;<sup>2</sup>&sdot;f<sup>2</sup>&sdot;L), F)
        TABLE
        VARIABLE(f) CAPTION(Book::tr("resonance frequency")) ROW
        VARIABLE(L) CAPTION(Book::tr("inductance")) ROW
        VARIABLE(C) CAPTION(Book::tr("capacitance")) ROW
        _TABLE
        END;
}

static QString makeGeometryCirclePage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Circle"))
        FORMULA(A = pi * r^2, A = &pi;&sdot;r<sup>2</sup>)
        FORMULA(A = pi * (d/2)^2, A = &pi;&sdot;(d/2)<sup>2</sup>)
        FORMULA(P = 2 * pi * r, P = 2&sdot;&pi;&sdot;r)
        FORMULA(P = pi * d, P = &pi;&sdot;d)
        TABLE
        VARIABLE(A) CAPTION(Book::tr("area")) ROW
        VARIABLE(P) CAPTION(Book::tr("perimeter")) ROW
        VARIABLE(d) CAPTION(Book::tr("diameter")) ROW
        VARIABLE(r) CAPTION(Book::tr("radius"))
        _TABLE
        END;
}

static QString makeGeometryConePage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Cone"))
        FORMULA(V = 1/3 * pi * r%5e2 * h, V = (1/3)&sdot;pi&sdot;r<sup>2</sup>&sdot;h)
        FORMULA(A = pi * r%5e2 + pi * r * s, A = &pi;&sdot;r<sup>2</sup> + &pi;&sdot;r&sdot;s)
        FORMULA(A = pi * r%5e2 + pi * r * sqrt(r%5e2+h%5e2), A = &pi;&sdot;r<sup>2</sup> + &pi;&sdot;r&sdot;&radic;(r<sup>2</sup>+h<sup>2</sup>))
        TABLE
        VARIABLE(V) CAPTION(Book::tr("volume")) ROW
        VARIABLE(A) CAPTION(Book::tr("surface area")) ROW
        VARIABLE(r) CAPTION(Book::tr("radius")) ROW
        VARIABLE(h) CAPTION(Book::tr("height")) ROW
        VARIABLE(s) CAPTION(Book::tr("slant height"))
        _TABLE
        END;
}

static QString makeGeometryCubePage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Cube"))
        FORMULA(V = l%5e3, V = l<sup>3</sup>)
        FORMULA(A = 6 * l%5e2, A = 6&sdot;l<sup>2</sup>)
        FORMULA(Df = sqrt(2) * l, D<sub>f</sub> = &radic;2&sdot;l)
        FORMULA(Ds = sqrt(3) * l, D<sub>s</sub> = &radic;3&sdot;l)
        TABLE
        VARIABLE(V) CAPTION(Book::tr("volume")) ROW
        VARIABLE(A) CAPTION(Book::tr("surface area")) ROW
        VARIABLE(D<sub>f</sub>) CAPTION(Book::tr("face diagonal")) ROW
        VARIABLE(D<sub>s</sub>) CAPTION(Book::tr("space diagonal")) ROW
        VARIABLE(l) CAPTION(Book::tr("edge length"))
        _TABLE
        END;
}

static QString makeGeometryCylinderPage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Cylinder"))
        FORMULA(V = pi * r%5e2 * h, V = &pi;&sdot;r<sup>2</sup>&sdot;h)
        FORMULA(A = (2 * pi * r%5e2) + (2 * pi * r * h), A = 2&sdot;&pi;&sdot;r<sup>2</sup> + 2&sdot;&pi;&sdot;r&sdot;h)
        TABLE
        VARIABLE(V) CAPTION(Book::tr("volume")) ROW
        VARIABLE(A) CAPTION(Book::tr("area")) ROW
        VARIABLE(r) CAPTION(Book::tr("radius")) ROW
        VARIABLE(h) CAPTION(Book::tr("height"))
        _TABLE
        END;
}

static QString makeGeometrySectorPage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Sector"))
        FORMULA(A = 1/2 * L * r, A = 1/2&sdot;L&sdot;r)
        FORMULA(A = theta/360 * pi * r^2, A = &theta;/360&sdot;&pi;&sdot;r<sup>2</sup>)
        FORMULA(L = theta/180 * pi * r, L = &theta;/180&sdot;&pi;&sdot;r)
        TABLE
        VARIABLE(A) CAPTION(Book::tr("area")) ROW
        VARIABLE(L) CAPTION(Book::tr("arc length")) ROW
        VARIABLE(r) CAPTION(Book::tr("radius")) ROW
        VARIABLE(&theta;) CAPTION(Book::tr("central angle (degrees)"))
        _TABLE
        END;
}

static QString makeGeometrySpherePage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Sphere"))
        FORMULA(A = 4 * pi * r^2, A = 4&sdot;&pi;&sdot;r<sup>2</sup>)
        FORMULA(A = pi *d^2, A = &pi;&sdot;d<sup>2</sup>)
        FORMULA(V = 4/3 * pi * r^3, V = 4/3&sdot;&pi;&sdot;r<sup>3</sup>)
        FORMULA(V = 1/6 * pi * d^3, V = 1/6&sdot;&pi;&sdot;d<sup>3</sup>)
        TABLE
        VARIABLE(A) CAPTION(Book::tr("area")) ROW
        VARIABLE(V) CAPTION(Book::tr("volume")) ROW
        VARIABLE(r) CAPTION(Book::tr("radius")) ROW
        VARIABLE(d) CAPTION(Book::tr("diameter"))
        _TABLE
        END;
}

static QString makeRFAntennasPage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Radio Frequency"))
        FORMULA_UNIT(Pd = Pin / (4 * pi * r%5e2), Pd = P<sub>in</sub> / (4&sdot;&pi;&sdot;r<sup>2</sup>), W&sdot;m<sup>-2</sup>)
        FORMULA_UNIT(E = sqrt(Pd * 120 * pi), E = &radic;(P<sub>d</sub>&sdot;120&sdot;&pi;), V&sdot;m<sup>-1</sup>)
        FORMULA_UNIT(E = sqrt((Pin * 120 * pi) / (4 * pi * r%5e2)), E = &radic;[(P<sub>in</sub>&sdot;120&sdot;&pi;) / (4&sdot;&pi;&sdot;r<sup>2</sup>)], V&sdot;m<sup>-1</sup>)
        FORMULA_UNIT(E = sqrt(30 * Pin) / r, E = &radic;(30&sdot;P<sub>in</sub>) / r, V&sdot;m<sup>-1</sup>)
        FORMULA_UNIT(Ed = E * sqrt(D), Ed = E&sdot;&radic;D, V&sdot;m<sup>-1</sup>)
        FORMULA_UNIT(Edp = E * sqrt(1.64), Edp = E&sdot;&radic;1.64, V&sdot;m<sup>-1</sup>)
        TABLE
        VARIABLE(T<sub>c</sub></sub>) CAPTION(Book::tr("temperature (C)")) ROW
        VARIABLE(T<sub>f</sub>) CAPTION(Book::tr("temperature (F)")) ROW
        VARIABLE(T<sub>k</sub>) CAPTION(Book::tr("temperature (K)"))
        _TABLE
        END;
}

static QString makeRFImpedancePage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Characteristic Impedance (coax)"))
        FORMULA_UNIT(Zo = 59.959 / sqrt(er) * ln(d1/d2), Z<sub>0</sub> = 59.959 / &radic;e<sub>r</sub> &sdot; ln(d<sub>1</sub>/d<sub>2</sub>), &Omega;)
        TABLE
        VARIABLE(e<sub>r) CAPTION(Book::tr("dielectric constant")) ROW
        VARIABLE(d<sub>1) CAPTION(Book::tr("outer conductor's inner diameter")) ROW
        VARIABLE(d<sub>2</sub>) CAPTION(Book::tr("inner conductor's outer diameter"))
        _TABLE
        END;
}

static QString makeRFPropagationPage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Velocity of Propagation (coax)"))
        SUBTITLE(V<sub>p</sub> = c / &radic;e<sub>r</sub>)
        FORMULA_UNIT(Vp = 3e8 / sqrt(er), V<sub>p</sub> = 3e8 / &radic;e<sub>r</sub>, m&sdot;s<sup>-1</sup>)
        FORMULA_UNIT(Vp = 299792458 / sqrt(er), V<sub>p</sub> = 299792458 / &radic;e<sub>r</sub>, m&sdot;s<sup>-1</sup>)
        FORMULA_UNIT(Vp = 983571056.43045 / sqrt(er), V<sub>p</sub> = 983571056.43045 / &radic;e<sub>r</sub>, ft&sdot;s<sup>-1</sup>)
        TABLE
        VARIABLE(c) CAPTION(Book::tr("speed of light")) ROW
        VARIABLE(e) CAPTION(Book::tr("dielectric constant")) ROW
        _TABLE
        END;
}

static QString makeRFSWRPage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Standing Wave Ratio & Return Loss"))
        FORMULA(SWR = (1+r) / (1-r), SWR = (1 + &rho;) / (1 - &rho;))
        FORMULA(r = (SWR-1) / (SWR+1), &rho; = (SWR - 1) / (SWR + 1))
        FORMULA(r = (Z-1) / (Z+1), &rho; = (Z - 1) / (Z + 1))
        FORMULA_UNIT(Z = (1+r) / (1-r), Z = (1 + &rho;) / (1 - &rho;), &Omega;)
        FORMULA_UNIT(RL = -20 * log(r), R<sub>L</sub> = -20&sdot;log(&rho;), dB)
        FORMULA_UNIT(RL = -20 * log((SWR-1)/(SWR+1)), R<sub>L</sub> = -20&sdot;log[(SWR - 1) / (SWR + 1)], dB)
        FORMULA_UNIT(Pr = Pin * r%5e2, P<sub>r</sub> = P<sub>in</sub>&sdot;&rho;<sup>2</sup>, W)
        FORMULA_UNIT(Pt = Pin * (1-r%5e2), P<sub>t</sub> = P<sub>in</sub>&sdot;(1-&rho;<sup>2</sup>), W)
        TABLE
        VARIABLE(P<sub>in</sub>) CAPTION(Book::tr("input power (W)")) ROW
        VARIABLE(P<sub>r</sub>) CAPTION(Book::tr("reflected power (W)")) ROW
        VARIABLE(P<sub>t</sub>) CAPTION(Book::tr("transmitted power (W)")) ROW
        VARIABLE(R<sub>L</sub>) CAPTION(Book::tr("return loss (dB)")) ROW
        VARIABLE(&rho;) CAPTION(Book::tr("reflection coefficient")) ROW
        VARIABLE(SWR) CAPTION(Book::tr("standing wave ratio")) ROW
        VARIABLE(Z) CAPTION(Book::tr("normalized impedance")) ROW
        _TABLE
        END;
}

static QString makeRFWavelengthPage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Free Space Wavelength"))
        SUBTITLE(&lambda;<sub>0</sub> = c / f)
        FORMULA_UNIT(L = 3e8 / f, L = 3&times;10<sup>8</sup> / f, m)
        FORMULA_UNIT(L = 299792458 / f, L = 299792458 / f, m)
        FORMULA_UNIT(L = 983571056.43045 / f, L = 983571056.43045 / f, ft)
        TABLE
        VARIABLE(f) CAPTION(Book::tr("frequency (Hz)")) ROW
        _TABLE
        END;
}

static QString makeUnitsTemperaturePage()
{
    return
        BEGIN
        INDEX_LINK
        TITLE(Book::tr("Temperature"))
        FORMULA(Tf = 1.8 * Tc + 32, T<sub>f</sub> = 1.8 * T<sub>c</sub> + 32)
        FORMULA(Tk = Tc + 273.15, T<sub>k</sub> = T<sub>c</sub> + 273.15)
        FORMULA(Tc = (Tf - 32) / 1.8, T<sub>c</sub> = (T<sub>f</sub> - 32) / 1.8)
        FORMULA(Tc = Tk - 273.15, T<sub>c</sub> = T<sub>k</sub> - 273.15)
        TABLE
        VARIABLE(T<sub>c</sub></sub>) CAPTION(Book::tr("temperature (C)")) ROW
        VARIABLE(T<sub>f</sub>) CAPTION(Book::tr("temperature (F)")) ROW
        VARIABLE(T<sub>k</sub>) CAPTION(Book::tr("temperature (K)"))
        _TABLE
        END;
}

Book::Book(QObject* parent)
    : QObject(parent)
{
    m_toc["index"] = &makeIndexPage;
    m_toc["algebra/quadratic-equation"] = &makeAlgebraQuadraticEquationPage;
    m_toc["algebra/log-base-conversion"] = &makeAlgebraLogBaseConversionPage;
    m_toc["electronics/ohmslaw"] = &makeElectronicsOhmsLawPage;
    m_toc["electronics/power"] = &makeElectronicsPowerPage;
    m_toc["electronics/reactance"] = &makeElectronicsReactancePage;
    m_toc["electronics/resonance"] = &makeElectronicsResonancePage;
    m_toc["geometry/circle"] = &makeGeometryCirclePage;
    m_toc["geometry/cone"] = &makeGeometryConePage;
    m_toc["geometry/cube"] = &makeGeometryCubePage;
    m_toc["geometry/cylinder"] = &makeGeometryCylinderPage;
    m_toc["geometry/sector"] = &makeGeometrySectorPage;
    m_toc["geometry/sphere"] = &makeGeometrySpherePage;
    m_toc["rf/antennas"] = &makeRFAntennasPage;
    m_toc["rf/impedance"] = &makeRFImpedancePage;
    m_toc["rf/propagation"] = &makeRFPropagationPage;
    m_toc["rf/swr"] = &makeRFSWRPage;
    m_toc["rf/wavelength"] = &makeRFWavelengthPage;
    m_toc["units/temperature"] = &makeUnitsTemperaturePage;
}

QString Book::getPageContent(const QString& id)
{
    PageMaker maker = m_toc.value(id);
    return maker ? maker() : QString();
}