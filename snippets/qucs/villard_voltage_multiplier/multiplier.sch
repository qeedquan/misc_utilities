<Qucs Schematic 24.2.1>
<Properties>
  <View=-815,-270,1457,939,1.16089,966,0>
  <Grid=10,10,1>
  <DataSet=multiplier.dat>
  <DataDisplay=multiplier.dpl>
  <OpenDisplay=0>
  <Script=multiplier.m>
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
  <C C1 1 290 140 -27 -52 0 2 "1 uF" 1 "" 0 "neutral" 0>
  <C C2 1 350 240 -26 17 1 2 "1 uF" 1 "" 0 "neutral" 0>
  <C C3 1 470 140 -26 -50 0 2 "1 uF" 1 "" 0 "neutral" 0>
  <C C4 1 530 240 -26 17 1 2 "1 uF" 1 "" 0 "neutral" 0>
  <R R1 1 130 140 -28 -56 1 0 "1 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 100 260 0 0 0 0>
  <GND * 1 640 320 0 0 0 0>
  <R R_load 1 640 280 16 -18 0 1 "100 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vac V1 1 100 200 18 -26 0 1 "10 V" 1 "1 kHz" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <.TR TR1 1 90 350 0 86 0 0 "lin" 1 "0" 1 "10ms" 1 "101" 1 "Gear" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Diode D_1N4004_1 5 320 190 -13 -26 0 3 "76.9p" 0 "1.45" 0 "39.8p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "42m" 0 "4.32u" 0 "0" 0 "0" 0 "1" 0 "1" 0 "400" 0 "5u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Diode D_1N4004_2 5 380 190 13 -26 0 1 "76.9p" 0 "1.45" 0 "39.8p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "42m" 0 "4.32u" 0 "0" 0 "0" 0 "1" 0 "1" 0 "400" 0 "5u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Diode D_1N4004_3 5 500 190 -13 -26 0 3 "76.9p" 0 "1.45" 0 "39.8p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "42m" 0 "4.32u" 0 "0" 0 "0" 0 "1" 0 "1" 0 "400" 0 "5u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Diode D_1N4004_4 5 560 190 13 -26 0 1 "76.9p" 0 "1.45" 0 "39.8p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "42m" 0 "4.32u" 0 "0" 0 "0" 0 "1" 0 "1" 0 "400" 0 "5u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <IProbe Pr1 1 350 140 -26 16 0 0>
  <IProbe Pr2 1 530 140 -26 16 0 0>
</Components>
<Wires>
  <320 140 320 160 "" 0 0 0 "">
  <320 220 320 240 "" 0 0 0 "">
  <380 140 380 160 "" 0 0 0 "">
  <380 220 380 240 "" 0 0 0 "">
  <500 220 500 240 "" 0 0 0 "">
  <500 140 500 160 "" 0 0 0 "">
  <560 220 560 240 "" 0 0 0 "">
  <560 140 560 160 "" 0 0 0 "">
  <380 240 500 240 "" 0 0 0 "">
  <380 140 440 140 "" 0 0 0 "">
  <100 240 320 240 "" 0 0 0 "">
  <160 140 260 140 "Input" 200 90 16 "">
  <100 240 100 260 "" 0 0 0 "">
  <640 240 640 250 "" 0 0 0 "">
  <560 240 640 240 "Output" 640 190 61 "">
  <640 310 640 320 "" 0 0 0 "">
  <100 230 100 240 "" 0 0 0 "">
  <100 140 100 170 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 820 473 534 393 3 #c0c0c0 1 00 1 0 0.001 0.01 1 -12.0094 5 11.9977 1 -3.14389 5 35 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(input)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(output)" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 800 874 596 324 3 #c0c0c0 1 00 1 0 0.001 0.01 1 -1 0.5 1 1 -0.047594 0.02 0.0434645 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.i(pr2)@time" #ff0000 0 3 0 0 1>
  </Rect>
</Diagrams>
<Paintings>
  <Rectangle 250 80 170 220 #000000 0 1 #c0c0c0 1 0>
  <Rectangle 430 80 170 220 #000000 0 1 #c0c0c0 1 0>
  <Text 90 310 12 #ff0000 0 "Generator">
  <Text 310 310 12 #ff0000 0 "1. stage">
  <Text 500 310 12 #ff0000 0 "2. stage">
  <Rectangle 80 80 100 210 #000000 0 1 #c0c0c0 1 0>
  <Text 260 -130 12 #000000 0 "Two-stage Villard circuit (Voltage Multiplier):\nEvery stage adds two-times the input voltage.\n(Decreased by the diode voltage drop)\nMany stages can be put in cascade.\nThis circuit can only drive a high-impedance load.\n\nSince this is an active component, the current is not inversely proportional to the voltage gain, unlike a transformer.">
</Paintings>
