<Qucs Schematic 24.2.1>
<Properties>
  <View=-406,34,1573,816,1.21,369,60>
  <Grid=10,10,1>
  <DataSet=inverting_amplifier.dat>
  <DataDisplay=inverting_amplifier.dpl>
  <OpenDisplay=0>
  <Script=inverting_amplifier.m>
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
  <R R2 1 420 90 -26 15 0 0 "3Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R1 1 280 90 -26 15 0 0 "1Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Vac V1 1 200 200 18 -26 0 1 "5V" 1 "40" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 200 280 0 0 0 0>
  <OpAmp OP1 1 590 190 -26 -91 1 0 "1e6" 1 "15 V" 1>
  <.TR TR1 1 510 350 0 74 0 0 "lin" 1 "0" 1 "100ms" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <200 90 200 170 "" 0 0 0 "">
  <200 90 250 90 "Vin" 230 60 2 "">
  <310 90 350 90 "" 0 0 0 "">
  <450 90 690 90 "" 0 0 0 "">
  <690 90 690 190 "" 0 0 0 "">
  <630 190 690 190 "" 0 0 0 "">
  <350 90 390 90 "" 0 0 0 "">
  <350 90 350 170 "" 0 0 0 "">
  <350 170 560 170 "" 0 0 0 "">
  <460 210 560 210 "" 0 0 0 "">
  <460 210 460 250 "" 0 0 0 "">
  <200 230 200 250 "" 0 0 0 "">
  <200 250 200 280 "" 0 0 0 "">
  <200 250 460 250 "" 0 0 0 "">
  <690 190 690 190 "Vout" 720 160 0 "">
</Wires>
<Diagrams>
  <Rect 40 573 323 243 3 #c0c0c0 1 00 1 0 0.02 0.1 1 -20 20 20 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vin)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 90 680 9 #000000 0 "Inverting amplifier outputs a signal that is scaled by a negative factor (flipping the signal from where it is positive to negative and vice versa)\nThe resistor can be adjusted to change the gain, there are some configurations of the resistors where it can create a square wave">
</Paintings>
