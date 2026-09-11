<Qucs Schematic 24.2.1>
<Properties>
  <View=68,-29,1487,770,1.10889,0,0>
  <Grid=10,10,1>
  <DataSet=diode_or_gate.dat>
  <DataDisplay=diode_or_gate.dpl>
  <OpenDisplay=0>
  <Script=x.m>
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
  <Diode D1 1 340 90 -26 -79 0 2 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND * 1 310 260 0 0 0 0>
  <Diode D2 1 340 420 -26 -79 0 2 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND * 1 310 590 0 0 0 0>
  <R R1 1 540 290 -26 15 0 0 "1 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <VProbe Pr1 1 540 220 28 -31 0 0>
  <GND * 1 610 350 0 0 0 0>
  <.TR TR1 1 830 50 0 76 0 0 "lin" 1 "0" 1 "1" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vdc V2 1 310 520 18 -26 0 1 "1V" 1>
  <Vdc V1 1 310 190 18 -26 0 1 "5V" 1>
</Components>
<Wires>
  <310 90 310 160 "V1I" 340 110 51 "">
  <310 220 310 260 "" 0 0 0 "">
  <310 420 310 490 "V2I" 340 430 41 "">
  <310 550 310 590 "" 0 0 0 "">
  <370 90 460 90 "" 0 0 0 "">
  <460 90 460 290 "" 0 0 0 "">
  <460 290 490 290 "" 0 0 0 "">
  <370 420 460 420 "" 0 0 0 "">
  <460 290 460 420 "" 0 0 0 "">
  <570 290 580 290 "" 0 0 0 "">
  <490 240 530 240 "" 0 0 0 "">
  <490 290 510 290 "" 0 0 0 "">
  <490 240 490 290 "" 0 0 0 "">
  <550 240 580 240 "" 0 0 0 "">
  <580 240 580 290 "" 0 0 0 "">
  <580 290 610 290 "" 0 0 0 "">
  <610 290 610 350 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 720 585 595 355 3 #c0c0c0 1 00 1 0 0.1 1 0 0 1 6 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(v1i)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(v2i)" #ff0000 0 3 0 0 0>
	<"ngspice/tran.v(pr1)" #ff00ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 240 680 9 #000000 0 "An OR gate can be made out of diodes.\nThe diode acts like a gate for the voltage sources, it will be low voltage output until one of voltage sources activates, then the output will be high\nThe output will be some voltage (assume we given it enough voltage for it to pass the high threshold) that matches the OR gate truth table.">
</Paintings>
