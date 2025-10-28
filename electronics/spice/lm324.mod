* WARNING : please consider following remarks before usage
*
* 1) All models are a tradeoff between accuracy and complexity (ie. simulation 
*    time).
* 2) Macromodels are not a substitute to breadboarding, they rather confirm the
*    validity of a design approach and help to select surrounding component values.
*
* 3) A macromodel emulates the NOMINAL performance of a TYPICAL device within 
*    SPECIFIED OPERATING CONDITIONS (ie. temperature, supply voltage, etc.).
*    Thus the macromodel is often not as exhaustive as the datasheet, its goal
*    is to illustrate the main parameters of the product.
*
* 4) Data issued from macromodels used outside of its specified conditions
*    (Vcc, Temperature, etc) or even worse: outside of the device operating 
*    conditions (Vcc, Vicm, etc) are not reliable in any way.
*
*
** Standard Linear Ics Macromodels, 1993. 
** CONNECTIONS :
* 1 INVERTING INPUT
* 2 NON-INVERTING INPUT
* 3 OUTPUT
* 4 POSITIVE POWER SUPPLY
* 5 NEGATIVE POWER SUPPLY
.SUBCKT LM324 2 1 4 5 3
***************************
.MODEL MDTH D IS=1E-8 KF=3.104131E-15 CJO=10F
* INPUT STAGE
CIP 2 5 1.000000E-12
CIN 1 5 1.000000E-12
EIP 10 5 2 5 1
EIN 16 5 1 5 1
RIP 10 11 2.600000E+01
RIN 15 16 2.600000E+01
RIS 11 15 2.003862E+02
DIP 11 12 MDTH 400E-12
DIN 15 14 MDTH 400E-12
VOFP 12 13 DC 0 
VOFN 13 14 DC 0
IPOL 13 5 1.000000E-05
CPS 11 15 3.783376E-09
DINN 17 13 MDTH 400E-12
VIN 17 5 0.000000e+00
DINR 15 18 MDTH 400E-12
VIP 4 18 2.000000E+00
FCP 4 5 VOFP 3.400000E+01
FCN 5 4 VOFN 3.400000E+01
FIBP 2 5 VOFN 2.000000E-03
FIBN 5 1 VOFP 2.000000E-03
* AMPLIFYING STAGE
FIP 5 19 VOFP 3.600000E+02
FIN 5 19 VOFN 3.600000E+02
RG1 19 5 3.652997E+06
RG2 19 4 3.652997E+06
CC 19 5 6.000000E-09
DOPM 19 22 MDTH 400E-12
DONM 21 19 MDTH 400E-12
HOPM 22 28 VOUT 7.500000E+03
VIPM 28 4 1.500000E+02
HONM 21 27 VOUT 7.500000E+03
VINM 5 27 1.500000E+02
EOUT 26 23 19 5 1
VOUT 23 5 0
ROUT 26 3 20
COUT 3 5 1.000000E-12
DOP 19 25 MDTH 400E-12
VOP 4 25 2.242230E+00
DON 24 19 MDTH 400E-12
VON 24 5 7.922301E-01
.ENDS
