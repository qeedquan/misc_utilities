<Qucs Schematic 24.2.1>
<Properties>
  <View=-419,-284,1247,795,0.944328,0,120>
  <Grid=10,10,1>
  <DataSet=dc_voltage_square.dat>
  <DataDisplay=dc_voltage_square.dpl>
  <OpenDisplay=0>
  <Script=dc_pulse_voltage.m>
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
  <R R1 1 40 140 -26 15 0 0 "1 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 30 300 0 0 0 0>
  <Vpulse V1 1 -120 170 18 -26 0 1 "0 V" 1 "1 V" 1 "0.5" 1 "3" 1 "1 ns" 0 "1 ns" 0>
  <.TR TR1 1 -130 460 0 79 0 0 "lin" 1 "0" 1 "5" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R R2 1 420 140 -26 15 0 0 "1 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 410 300 0 0 0 0>
  <Vrect V2 1 260 170 18 -26 0 1 "1 V" 1 "1" 1 "0.5" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0 V" 1>
</Components>
<Wires>
  <-120 140 10 140 "VPULSE_MONITOR" -30 60 83 "">
  <70 140 170 140 "" 0 0 0 "">
  <170 140 170 300 "" 0 0 0 "">
  <30 300 170 300 "" 0 0 0 "">
  <-120 200 -120 300 "" 0 0 0 "">
  <-120 300 30 300 "" 0 0 0 "">
  <260 140 390 140 "VRECT_MONITOR" 310 80 49 "">
  <450 140 550 140 "" 0 0 0 "">
  <550 140 550 300 "" 0 0 0 "">
  <410 300 550 300 "" 0 0 0 "">
  <260 200 260 300 "" 0 0 0 "">
  <260 300 410 300 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect -90 -80 240 160 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vpulse_monitor)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 280 -80 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vrect_monitor)@time" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 260 400 12 #000000 0 "A pulse voltage source is an injection of a rectangle signal that last for a brief moment of time.\nA rectangular voltage is like a pulse voltage but it repeats\n\nWe can use transient simulation to show what happens to the signal over time">
</Paintings>
