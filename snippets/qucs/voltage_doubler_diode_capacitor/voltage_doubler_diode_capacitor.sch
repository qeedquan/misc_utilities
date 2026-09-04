<Qucs Schematic 24.2.1>
<Properties>
  <View=-535,-625,1480,825,0.944546,163,0>
  <Grid=10,10,1>
  <DataSet=voltage_doubler_diode_capacitor.dat>
  <DataDisplay=voltage_doubler_diode_capacitor.dpl>
  <OpenDisplay=0>
  <Script=voltage_doubler.m>
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
  <Diode D3 1 370 70 -26 -103 0 2 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Diode D2 1 280 190 -93 -26 0 3 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND * 1 350 320 0 0 0 0>
  <C C1 1 220 70 -26 -61 0 2 "10 uF" 1 "" 0 "neutral" 0>
  <C C2 1 430 190 17 -26 0 1 "10 uF" 1 "" 0 "neutral" 0>
  <R R1 1 580 170 15 -26 0 1 "115Kohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vac V1 1 80 150 18 -26 0 1 "5.555V" 1 "80" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <.TR TR1 1 830 80 0 79 0 0 "lin" 1 "0" 1 "10" 1 "2000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <430 70 580 70 "" 0 0 0 "">
  <580 70 580 140 "Vout" 610 70 33 "">
  <580 200 580 280 "" 0 0 0 "">
  <280 70 340 70 "" 0 0 0 "">
  <400 70 430 70 "" 0 0 0 "">
  <280 70 280 160 "" 0 0 0 "">
  <280 220 280 280 "" 0 0 0 "">
  <280 280 350 280 "" 0 0 0 "">
  <350 280 430 280 "" 0 0 0 "">
  <350 280 350 320 "" 0 0 0 "">
  <80 280 280 280 "" 0 0 0 "">
  <80 70 80 120 "" 0 0 0 "">
  <80 70 190 70 "Vin" 140 40 29 "">
  <250 70 280 70 "" 0 0 0 "">
  <430 280 580 280 "" 0 0 0 "">
  <430 70 430 160 "" 0 0 0 "">
  <430 220 430 280 "" 0 0 0 "">
  <80 180 80 280 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 30 724 989 294 3 #c0c0c0 1 00 1 0 0.5 10 1 -0.000556745 0.001 0.00430812 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vout)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -210 -380 12 #000000 0 "A voltage doubler takes in AC signal and produce a DC signal that has the magnitude of the peak AC voltage (in the best case).\nThis circuit really needs to take advantage of feedback from capacitors charging, so it is frequency dependent on how well it can perform.\nThe capacitor and resistor value determines what frequencies it works best at, since we only have constant capacitor/resistor values, different frequencies need component tuning.\n\nThe circuit only works well in a range of frequencies (needs to be at higher frequencies).\nOutside of the valid range, the output will not be able to achieve 2x voltage output but a fraction of it or even goes to zero.\nFor DC inputs, this circuit outputs zero voltage as time goes to infinity.\n\nWith these restrictions, it is misleading to call this a voltage doubler since naively one would think it works on all inputs.\nBut this is the standard terminology when people talk about a voltage doubler.\n\nThe simulation also needs to run for a long time, if the capacitor/resistor values are not the right one for the doubler, the voltage doubler output will not sustain as time goes forward.">
</Paintings>
