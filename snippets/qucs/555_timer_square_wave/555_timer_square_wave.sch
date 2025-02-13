<Qucs Schematic 24.2.1>
<Properties>
  <View=49,-60,1175,574,1.39748,0,0>
  <Grid=10,10,1>
  <DataSet=555_timer_square_wave.dat>
  <DataDisplay=555_timer_square_wave.dpl>
  <OpenDisplay=0>
  <Script=555timer_osc.m>
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
  <GND * 1 280 360 0 0 0 0>
  <Vdc V1 1 360 20 18 -26 0 1 "5" 1>
  <GND * 1 360 50 0 0 0 0>
  <GND * 1 390 380 0 0 0 0>
  <GND * 1 130 400 0 0 0 0>
  <R R3 1 130 110 15 -26 0 1 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 130 220 15 -26 0 1 "5k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 390 350 17 -26 0 1 "0.01u" 1 "" 0 "neutral" 0>
  <R R1 1 490 110 15 -26 0 1 "4.7k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C2 1 130 370 17 -26 0 1 "0.22u" 1 "" 0 "neutral" 0>
  <Lib SUB1 1 280 240 70 -96 0 0 "555_timer" 0 "NE555_bipolar" 0>
  <.TR TR1 1 810 10 0 75 0 0 "lin" 1 "0" 1 "50 ms" 1 "100000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 1 "0" 0>
</Components>
<Wires>
  <300 -20 360 -20 "" 0 0 0 "">
  <360 -20 360 -10 "" 0 0 0 "">
  <130 260 180 260 "" 0 0 0 "">
  <130 250 130 260 "" 0 0 0 "">
  <130 260 130 340 "" 0 0 0 "">
  <130 180 130 190 "" 0 0 0 "">
  <130 140 130 180 "" 0 0 0 "">
  <130 -20 130 80 "" 0 0 0 "">
  <130 -20 300 -20 "" 0 0 0 "">
  <360 -20 490 -20 "" 0 0 0 "">
  <490 -20 490 80 "" 0 0 0 "">
  <300 -20 300 70 "" 0 0 0 "">
  <130 180 200 180 "" 0 0 0 "">
  <490 140 490 240 "out" 520 190 82 "">
  <360 240 490 240 "" 0 0 0 "">
  <300 70 300 120 "" 0 0 0 "">
  <260 70 300 70 "" 0 0 0 "">
  <260 70 260 120 "" 0 0 0 "">
  <180 260 180 300 "" 0 0 0 "">
  <180 300 200 300 "" 0 0 0 "">
  <390 300 390 320 "" 0 0 0 "">
  <360 300 390 300 "" 0 0 0 "">
  <180 260 200 260 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 568 433 545 224 3 #c0c0c0 1 00 1 0 0.005 0.05 1 -0.473392 2 6 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/tran.v(out)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 110 500 12 #000000 0 "555 timer square wave oscillator example\n">
</Paintings>
