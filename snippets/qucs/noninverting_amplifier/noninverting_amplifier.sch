<Qucs Schematic 24.2.1>
<Properties>
  <View=-187,-80,1386,806,1,0,0>
  <Grid=10,10,1>
  <DataSet=noninverting_amplifier.dat>
  <DataDisplay=noninverting_amplifier.dpl>
  <OpenDisplay=0>
  <Script=noninverting_amplifier.m>
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
  <OpAmp OP1 1 520 140 -26 42 0 0 "1e6" 1 "15 V" 0>
  <Vac V1 1 230 160 18 -26 0 1 "5V" 1 "40" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <R R2 1 420 250 -26 15 0 0 "2K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R1 1 310 250 -26 15 0 0 "1K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 230 260 0 0 0 0>
  <.TR TR1 1 790 260 0 75 0 0 "lin" 1 "0" 1 "100 ms" 1 "2000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <230 120 230 130 "" 0 0 0 "">
  <230 120 490 120 "Vin" 320 90 55 "">
  <340 250 360 250 "" 0 0 0 "">
  <360 250 390 250 "" 0 0 0 "">
  <360 160 360 250 "" 0 0 0 "">
  <360 160 490 160 "" 0 0 0 "">
  <450 250 590 250 "" 0 0 0 "">
  <590 140 590 250 "" 0 0 0 "">
  <560 140 590 140 "" 0 0 0 "">
  <230 190 230 250 "" 0 0 0 "">
  <230 250 230 260 "" 0 0 0 "">
  <230 250 280 250 "" 0 0 0 "">
  <590 140 590 140 "Vout" 620 110 0 "">
</Wires>
<Diagrams>
  <Rect 740 180 240 160 3 #c0c0c0 1 00 1 0 0.02 0.1 1 -20 20 20 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vin)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 30 350 9 #000000 0 "Noninverting Amplifiers scale the output but a constant factor (defined by the resistor and opamp).\nIt has mostly the same behavior as a Inverting Amplifier, the main difference is that it does not invert the signal at the output.\n">
</Paintings>
