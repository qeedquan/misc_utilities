<Qucs Schematic 24.2.1>
<Properties>
  <View=-347,-20,1249,1197,0.985746,0,301>
  <Grid=10,10,1>
  <DataSet=active_mixer.dat>
  <DataDisplay=active_mixer.dpl>
  <OpenDisplay=0>
  <Script=active_mixer.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <_BJT T1 1 410 350 8 -26 0 0 "npn" 0 "1e-16" 0 "1" 0 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "200" 1 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <_BJT T3 1 470 270 -55 -26 1 2 "npn" 0 "1e-16" 0 "1" 0 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "200" 1 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <_BJT T2 1 350 270 8 -25 0 0 "npn" 0 "1e-16" 0 "1" 0 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "200" 1 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0.75" 0 "0.33" 0 "0" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <R R3 1 470 70 15 -26 0 1 "1.5k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 570 70 15 -26 0 1 "100k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R5 1 250 70 15 -26 0 1 "100k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 350 70 15 -26 0 1 "1.5k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 320 410 0 0 0 0>
  <GND * 1 410 410 0 0 0 0>
  <GND * 1 90 100 0 0 0 0>
  <GND * 1 90 410 0 0 0 0>
  <Vdc V2 1 90 70 18 -26 0 1 "10 V" 1>
  <Idc I2 1 320 380 18 -26 0 1 "2 uA" 1>
  <Iac RF 1 90 380 -74 -28 1 1 "1 uA" 1 "1 kHz" 1 "0" 0 "0" 0>
  <Vac LO 1 20 240 -92 -28 1 1 "0.5 V" 1 "10 kHz" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 20 290 0 0 0 0>
  <R R8 1 80 200 -28 -54 1 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 120 280 0 0 0 0>
  <C C1 1 640 110 -26 17 0 0 "1 F" 1 "" 0 "neutral" 0>
  <C C2 1 640 180 -26 17 0 0 "1 F" 1 "" 0 "neutral" 0>
  <GND * 1 780 190 0 0 0 0>
  <R R9 1 820 150 15 -26 0 1 "1.5k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 820 190 0 0 0 0>
  <Lib TRAN4 1 160 180 -20 100 0 0 "Transformers" 0 "PositiveCouplingPS" 0 "0.99" 1 "1" 1 "1" 1 "1" 1 "1" 1>
  <Lib TRAN5 1 720 90 -20 100 0 0 "Transformers" 0 "PositiveCouplingPS" 0 "0.99" 1 "1" 1 "1" 1 "1" 1 "1" 1>
  <.DC DC1 1 -50 530 0 45 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.TR TR1 1 -50 610 0 75 0 0 "lin" 1 "0" 1 "2 ms" 1 "501" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <350 300 410 300 "" 0 0 0 "">
  <410 300 470 300 "" 0 0 0 "">
  <410 300 410 320 "" 0 0 0 "">
  <570 20 570 40 "" 0 0 0 "">
  <470 20 570 20 "" 0 0 0 "">
  <470 20 470 40 "" 0 0 0 "">
  <350 20 470 20 "" 0 0 0 "">
  <350 20 350 40 "" 0 0 0 "">
  <250 20 350 20 "" 0 0 0 "">
  <250 20 250 40 "" 0 0 0 "">
  <350 100 350 110 "" 0 0 0 "">
  <250 100 250 270 "" 0 0 0 "">
  <250 270 320 270 "" 0 0 0 "">
  <570 100 570 200 "" 0 0 0 "">
  <500 270 570 270 "" 0 0 0 "">
  <410 380 410 410 "" 0 0 0 "">
  <90 20 90 40 "" 0 0 0 "">
  <90 20 250 20 "" 0 0 0 "">
  <90 350 320 350 "" 0 0 0 "">
  <320 350 380 350 "" 0 0 0 "">
  <20 270 20 290 "" 0 0 0 "">
  <570 200 570 270 "" 0 0 0 "">
  <20 200 20 210 "" 0 0 0 "">
  <20 200 50 200 "" 0 0 0 "">
  <350 110 350 240 "pos" 310 130 49 "">
  <470 100 470 180 "neg" 430 130 60 "">
  <470 180 470 240 "" 0 0 0 "">
  <350 110 610 110 "" 0 0 0 "">
  <470 180 610 180 "" 0 0 0 "">
  <780 180 780 190 "" 0 0 0 "">
  <760 180 780 180 "" 0 0 0 "">
  <820 180 820 190 "" 0 0 0 "">
  <760 110 820 110 "" 0 0 0 "">
  <820 110 820 120 "" 0 0 0 "">
  <200 270 250 270 "" 0 0 0 "">
  <200 200 570 200 "" 0 0 0 "">
  <110 200 140 200 "" 0 0 0 "">
  <120 270 120 280 "" 0 0 0 "">
  <120 270 140 270 "" 0 0 0 "">
  <670 110 700 110 "" 0 0 0 "">
  <670 180 700 180 "" 0 0 0 "">
  <820 110 820 110 "IF" 830 80 0 "">
</Wires>
<Diagrams>
  <Rect 160 851 342 371 3 #c0c0c0 1 00 1 0 0.0005 0.002 1 9 0.2 10.1099 1 -1 0.2 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(neg)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(pos)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 630 851 342 371 3 #c0c0c0 1 00 1 0 0.0002 0.001 1 9 0.2 10.1039 1 -1 0.2 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(if)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>