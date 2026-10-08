<Qucs Schematic 24.2.1>
<Properties>
  <View=-402,-281,1265,560,1.14175,207,0>
  <Grid=10,10,1>
  <DataSet=voltage_source_values_from_file.dat>
  <DataDisplay=voltage_source_values_from_file.dpl>
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
  <R R1 1 380 180 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 100 280 0 0 0 0>
  <R R2 1 790 170 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 510 270 0 0 0 0>
  <GND * 1 410 260 0 0 0 0>
  <GND * 1 820 250 0 0 0 0>
  <Vfile V1 1 100 230 18 -26 0 1 "values.dat" 1 "hold" 1 "yes" 1 "3" 0 "0" 0>
  <Vfile V2 1 510 220 18 -26 0 1 "values.dat" 1 "linear" 1 "yes" 1 "3" 0 "0" 0>
  <.TR TR1 1 860 -140 0 79 0 0 "lin" 1 "0" 1 "10" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <100 180 350 180 "Vin1" 230 150 104 "">
  <100 180 100 200 "" 0 0 0 "">
  <510 170 760 170 "Vin2" 640 140 104 "">
  <510 170 510 190 "" 0 0 0 "">
  <100 260 100 280 "" 0 0 0 "">
  <410 180 410 260 "" 0 0 0 "">
  <510 250 510 270 "" 0 0 0 "">
  <820 170 820 250 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 130 14 592 204 3 #c0c0c0 1 00 1 0 1 10 1 -0.6 2 6.6 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vin2)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 230 360 9 #000000 0 "Arbitrary waveforms can be used as an input.\nSpecify the data inside a file that can be loaded.\nThe format is:\n<t0> <v0>\n<t1> <v1>\n<t2> <v2>\n...\n\nThen the simulator has a variety of interpolation functions to map the points onto a curve.">
</Paintings>
