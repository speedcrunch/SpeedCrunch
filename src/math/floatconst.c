/* floatconst.c: constants for higher math functions */
/*
    Copyright (C) 2007, 2008 Wolf Lammen.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.


    You may contact the author by:
       e-mail:  ookami1 <at> gmx <dot> de
       mail:  Wolf Lammen
              Oertzweg 45
              22307 Hamburg
              Germany

*************************************************************************/

#include "floatconst.h"

static char sExp[] =
"2.7182818284""5904523536""0287471352""6624977572""4709369995"
  "9574966967""6277240766""3035354759""4571382178""5251664274"
  "2746639193""2003059921""8174135966""2904357290""0334295260"
  "5956307381""3232862794""3490763233""8298807531""9525101901"
  "1573834187";

static char sLn2[] =
"0.6931471805""5994530941""7232121458""1765680755""0013436025"
  "5254120680""0094933936""2196969471""5605863326""9964186875"
  "4200148102""0570685734";

static char sLn3[] =
"1.0986122886""6810969139""5245236922""5257046474""9055782274"
  "9451734694""3336374942""9321860896""6873615754""8137320887"
  "8797002906""5957865742";

static char sLn7[] =
"1.9459101490""5531330510""5352743443""1797296370""8472958186"
  "1188459390""1499375798""6275206926""7787658498""5878715269"
  "9306169420""5851140912";

static char sLn10[] =
"2.3025850929""9404568401""7991454684""3642076011""0148862877"
  "2976033327""9009675726""0967735248""0235997205""0895982983"
  "4196778404""2286248633";

static char sPhi[] =
"1.6180339887""4989484820""4586834365""6381177203""0917980576"
  "2862135448""6227052604""6281890244""9707207204""1893911374"
  "8475408807""5386891752""1266338622""2353693179""3180060766"
  "7263544333""8908659593""9582905638""3226613199""2829026788"
  "0675208766";

static char sPi[] =
"3.1415926535""8979323846""2643383279""5028841971""6939937510"
  "5820974944""5923078164""0628620899""8628034825""3421170679"
  "8214808651""3282306647""0938446095""5058223172""5359408128"
  "4811174502""8410270193""8521105559""6446229489""5493038196"
  "4428810976";

static char sPiDiv2[] =
"1.5707963267""9489661923""1321691639""7514420985""8469968755"
  "2910487472""2961539082""0314310449""9314017412""6710585339"
  "9107404325""6641153324";

static char sPiDiv4[] =
"0.7853981633""9744830961""5660845819""8757210492""9234984377"
  "6455243736""1480769541""0157155224""9657008706""3355292669"
  "9553702162""8320576662";

static char s2Pi[] =
"6.2831853071""7958647692""5286766559""0057683943""3879875021"
  "1641949889""1846156328""1257241799""7256069650""6842341359"
  "6429617302""6564613294";

static char s1DivPi[] =
"0.3183098861""8379067153""7767526745""0287240689""1929148091"
  "2897495334""6881177935""9526845307""0180227605""5325061719"
  "1214568545""3515916074";

static char sLnSqrt2PiMinusHalf[] =
"0.4189385332""0467274178""0329736405""6176398613""9747363778"
  "3412817151""5404827656""9592726039""7694743298""6359541976"
  "2200564662""4634337446";

static char sSqrtPi[] =
"1.7724538509""0551602729""8167483341""1451827975""4945612238"
  "7128213807""7898529112""8459103218""1374950656""7385446654"
  "1622682362""4282570666";

static char s1DivSqrtPi[] =
"0.5641895835""4775628694""8079451560""7725858440""5062932899"
  "8856844085""7217106424""6844149341""4486743660""2021073634"
  "4302834790""6361707352";

static char s2DivSqrtPi[] =
"1.1283791670""9551257389""6158903121""5451716881""0125865799"
  "7713688171""4434212849""3688298682""8973487320""4042147268"
  "8605669581""2723414703";

static char* sBernoulli[] =
{
"1",
"6",
"1",
"-30",
"1",
"42",
"1",
"-30",
"5",
"66",
"691",
"-2730",
"7",
"6",
"3617",
"-510",
"43867",
"798",
"174611",
"-330",
"854513",
"138",
"236364091",
"-2730",
"8553103",
"6",
"23749461029",
"-870",
"8615841276005",
"14322",
"7709321041217",
"-510",
"2577687858367",
"6",
"26315271553053477373",
"-1919190",
"2929993913841559",
"6",
"261082718496449122051",
"-13530",
"1520097643918070802691",
"1806",
"27833269579301024235023",
"-690",
"596451111593912163277961",
"282",
"5609403368997817686249127547",
"-46410",
"495057205241079648212477525",
"66",
"801165718135489957347924991853",
"-1590",
"29149963634884862421418123812691",
"798",
"2479392929313226753685415739663229",
"-870",
"84483613348880041862046775994036021",
"354",
"1215233140483755572040304994079820246041491",
"-56786730",
"12300585434086858541953039857403386151",
"6",
"106783830147866529886385444979142647942017",
"-510",
"1472600022126335654051619428551932342241899101",
"64722",
"78773130858718728141909149208474606244347001",
"-30",
"1505381347333367003803076567377857208511438160235",
"4686",
"5827954961669944110438277244641067365282488301844260429",
"-140100870",
"34152417289221168014330073731472635186688307783087",
"6",
"24655088825935372707687196040585199904365267828865801",
"-30",
"414846365575400828295179035549542073492199375372400483487",
"3318",
"4603784299479457646935574969019046849794257872751288919656867",
"-230010",
"1677014149185145836823154509786269900207736027570253414881613",
"498",
"2024576195935290360231131160111731009989917391198090877281083932477",
"-3404310",
"660714619417678653573847847426261496277830686653388931761996983",
"6",
"1311426488674017507995511424019311843345750275572028644296919890574047",
"-61410",
"1179057279021082799884123351249215083775254949669647116231545215727922535",
"272118",
"1295585948207537527989427828538576749659341483719435143023316326829946247",
"-1410",
"1220813806579744469607301679413201203958508415202696621436215105284649447",
"6",
"-4.70038339580357310785752555350060606545967373697590579151397635641e73",
"1",
"1.13180434454842492706751862577339342678903659547507479181789935417e76",
"1",
"-2.83822495706937069592641563364817647382846809280128821282285317145e78",
"1",
"7.40642489796788506297508271409209841768797317880887066731161003487e80",
"1",
"-2.00964548027566044834656196727153631868672708225328766243461301989e83",
"1",
"5.66571700508059414457193460305193569614194682875104206213875644522e85",
"1",
"-1.65845111541362169158237133743199123014949626147254647274024668156e88",
"1",
"5.03688599504923774192894219151801548124423742649032141415256513225e90",
"1",
"-1.58614682376581863693634015729664387827409784127789638804728645143e93",
"1",
"5.17567436175456269840732406825071225612408492359305508590621669403e95",
"1",
"-1.74889218402171173396900258776181591451414761618265448726273472159e98",
"1",
"6.11605199949521852558245252642641677807677268467832007168432401127e100",
"1",
"-2.2122776912707834942288323456712932445573185054987780150566552693e103",
"1",
"8.27227767987709698542210624599845957312046505184335662838488529886e105",
"1",
"-3.19589251114157095835916343691808148735262766710991122731845042431e108",
"1",
"1.27500822233877929823100243029266798669571917963897732951605857354e111",
"1",
"-5.25009230867741338994028246245651754469198940377552432607801345222e113",
"1",
"2.2301817894241625209869298198838728143738272150875878542490550781e116",
"1",
"-9.76845219309552044386335133989802393011669026749856789710001706619e118",
"1",
"4.40983619784529542722726228748131691918757542655281147353197591401e121",
"1",
"-2.05085708864640888397293377275830154864565966904008359530873982755e124",
"1"
};

floatstruct cBernoulliNum[68];
floatstruct cBernoulliDen[68];

floatstruct c1;
floatstruct c2;
floatstruct c3;
floatstruct c12;
floatstruct c16;
floatstruct cExp;
floatstruct cMinus1;
floatstruct cMinus20;
floatstruct c1Div2;
floatstruct cLn2;
floatstruct cLn3;
floatstruct cLn7;
floatstruct cLn10;
floatstruct cPhi;
floatstruct cPi;
floatstruct cPiDiv2;
floatstruct cPiDiv4;
floatstruct c2Pi;
floatstruct c1DivPi;
floatstruct cSqrtPi;
floatstruct cLnSqrt2PiMinusHalf;
floatstruct c1DivSqrtPi;
floatstruct c2DivSqrtPi;
floatstruct cMinus0_4;
floatstruct cUnsignedBound;

int erfcdigits = 0;
floatstruct erfccoeff[MAXERFCIDX];
floatstruct erfcalpha;
floatstruct erfcalphasqr;
floatstruct erfct2;
floatstruct erfct3;

void
floatmath_init()
{
  int i, save;
  floatnum_init();

  save = float_setprecision(MAXDIGITS);
  float_create(&c1);
  float_setinteger(&c1, 1);
  float_create(&c2);
  float_setinteger(&c2, 2);
  float_create(&c3);
  float_setinteger(&c3, 3);
  float_create(&c12);
  float_setinteger(&c12, 12);
  float_create(&c16);
  float_setinteger(&c16, 16);
  float_create(&cMinus1);
  float_setinteger(&cMinus1, -1);
  float_create(&cMinus20);
  float_setinteger(&cMinus20, -20);
  float_create(&c1Div2);
  float_setscientific(&c1Div2, ".5", NULLTERMINATED);
  float_create(&cExp);
  float_setscientific(&cExp, sExp, NULLTERMINATED);
  float_create(&cLn2);
  float_setscientific(&cLn2, sLn2, NULLTERMINATED);
  float_create(&cLn3);
  float_setscientific(&cLn3, sLn3, NULLTERMINATED);
  float_create(&cLn7);
  float_setscientific(&cLn7, sLn7, NULLTERMINATED);
  float_create(&cLn10);
  float_setscientific(&cLn10, sLn10, NULLTERMINATED);
  float_create(&cPhi);
  float_setscientific(&cPhi, sPhi, NULLTERMINATED);
  float_create(&cPi);
  float_setscientific(&cPi, sPi, NULLTERMINATED);
  float_create(&cPiDiv2);
  float_setscientific(&cPiDiv2, sPiDiv2, NULLTERMINATED);
  float_create(&cPiDiv4);
  float_setscientific(&cPiDiv4, sPiDiv4, NULLTERMINATED);
  float_create(&c2Pi);
  float_setscientific(&c2Pi, s2Pi, NULLTERMINATED);
  float_create(&c1DivPi);
  float_setscientific(&c1DivPi, s1DivPi, NULLTERMINATED);
  float_create(&cSqrtPi);
  float_setscientific(&cSqrtPi, sSqrtPi, NULLTERMINATED);
  float_create(&cLnSqrt2PiMinusHalf);
  float_setscientific(&cLnSqrt2PiMinusHalf, sLnSqrt2PiMinusHalf,
                      NULLTERMINATED);
  float_create(&c1DivSqrtPi);
  float_setscientific(&c1DivSqrtPi, s1DivSqrtPi, NULLTERMINATED);
  float_create(&c2DivSqrtPi);
  float_setscientific(&c2DivSqrtPi, s2DivSqrtPi, NULLTERMINATED);
  float_create(&cMinus0_4);
  float_setscientific(&cMinus0_4, "-.4", NULLTERMINATED);
  for (i = -1; ++i < MAXBERNOULLIIDX;)
  {
    float_create(&cBernoulliNum[i]);
    float_create(&cBernoulliDen[i]);
    float_setscientific(&cBernoulliNum[i], sBernoulli[2*i], NULLTERMINATED);
    float_setscientific(&cBernoulliDen[i], sBernoulli[2*i+1], NULLTERMINATED);
  }
  float_create(&cUnsignedBound);
  float_copy(&cUnsignedBound, &c1, EXACT);
  for (i = -1; ++i < 2*sizeof(unsigned);)
    float_mul(&cUnsignedBound, &c16, &cUnsignedBound, EXACT);
  for (i = -1; ++i < MAXERFCIDX;)
    float_create(&erfccoeff[i]);
  float_create(&erfcalpha);
  float_create(&erfcalphasqr);
  float_create(&erfct2);
  float_create(&erfct3);
  float_setprecision(save);
}

void
floatmath_exit()
{
  int i;

  float_free(&c1);
  float_free(&c2);
  float_free(&c3);
  float_free(&c12);
  float_free(&c16);
  float_free(&cMinus1);
  float_free(&cMinus20);
  float_free(&c1Div2);
  float_free(&cExp);
  float_free(&cLn2);
  float_free(&cLn3);
  float_free(&cLn7);
  float_free(&cLn10);
  float_free(&cPhi);
  float_free(&cPi);
  float_free(&cPiDiv2);
  float_free(&cPiDiv4);
  float_free(&c2Pi);
  float_free(&c1DivPi);
  float_free(&cSqrtPi);
  float_free(&cLnSqrt2PiMinusHalf);
  float_free(&c1DivSqrtPi);
  float_free(&c2DivSqrtPi);
  float_free(&cMinus0_4);
  for (i = -1; ++i < MAXBERNOULLIIDX;)
  {
    float_free(&cBernoulliNum[i]);
    float_free(&cBernoulliDen[i]);
  }
  float_free(&cUnsignedBound);
  for (i = -1; ++i < MAXERFCIDX;)
    float_free(&erfccoeff[i]);
  float_free(&erfcalpha);
  float_free(&erfcalphasqr);
  float_free(&erfct2);
  float_free(&erfct3);
}
