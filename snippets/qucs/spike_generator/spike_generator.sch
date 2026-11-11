<Qucs Schematic 24.2.1>
<Properties>
  <View=-1239,-825,2832,1207,0.683689,477,429>
  <Grid=10,10,1>
  <DataSet=spike_generator.dat>
  <DataDisplay=spike_generator.dpl>
  <OpenDisplay=0>
  <Script=spike_generator.m>
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
  <.TR TR1 1 940 20 0 79 0 0 "lin" 1 "0" 1 "1" 1 "10000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Diode D1 1 290 120 -26 -79 0 2 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND * 1 -60 260 0 0 0 0>
  <GND * 1 170 260 0 0 0 0>
  <C C1 1 70 120 -26 17 0 0 "10uF" 1 "" 0 "neutral" 0>
  <Vrect V1 1 -60 150 18 -26 0 1 "10V" 1 "25 ms" 1 "25 ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0 V" 1>
  <R R1 1 170 180 15 -26 0 1 "110Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr1 1 360 120 -26 16 0 0>
  <GND * 1 440 250 0 0 0 0>
  <R R2 1 440 180 15 -26 0 1 "100Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <320 120 330 120 "" 0 0 0 "">
  <-60 120 40 120 "Vin" 0 90 26 "">
  <-60 180 -60 260 "" 0 0 0 "">
  <100 120 170 120 "" 0 0 0 "">
  <170 120 260 120 "" 0 0 0 "">
  <170 120 170 150 "" 0 0 0 "">
  <170 210 170 260 "" 0 0 0 "">
  <440 120 440 150 "" 0 0 0 "">
  <440 210 440 250 "" 0 0 0 "">
  <390 120 440 120 "" 0 0 0 "">
  <440 120 550 120 "" 0 0 0 "">
  <550 120 550 120 "Vout" 580 90 0 "">
</Wires>
<Diagrams>
  <Rect 680 435 537 225 3 #c0c0c0 1 00 1 0 0.1 1 1 -1.0357 5 11.0032 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 670 706 582 226 3 #c0c0c0 1 00 1 0 0.1 1 1 -0.952589 5 11.589 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 660 1001 578 251 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@time" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -330 380 9 #000000 0 "A spike generator can be made using a square generator hooked to a capacitor, diode and resistor.\nC1 controls the spike width, a large value will make it look like the original square wave, while a smaller one makes it look more spike-like.\nD1 and R2 makes the negative spike output positive.\nThe spike voltage output is proportional to the voltage input, though too large of a voltage input needs the values of the other components to be changed to compensate.">
</Paintings>
