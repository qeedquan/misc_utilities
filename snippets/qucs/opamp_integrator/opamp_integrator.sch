<Qucs Schematic 24.2.1>
<Properties>
  <View=-804,-633,2564,805,0.826446,522,300>
  <Grid=10,10,1>
  <DataSet=opamp_integrator.dat>
  <DataDisplay=opamp_integrator.dpl>
  <OpenDisplay=0>
  <Script=opamp_integrator.m>
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
  <R R1 1 450 480 -26 15 0 0 "1 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 310 710 0 0 0 0>
  <C C1 1 590 480 -26 17 0 0 "5.8 u" 1 "" 0 "neutral" 0>
  <OpAmp OP1 1 650 590 -26 -74 1 0 "1e6" 1 "15 V" 0>
  <Vrect V1 1 310 510 18 -26 0 1 "2V" 1 "12.5 ms" 1 "12.5 ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "-2V" 1>
  <Vrect V2 1 310 620 18 -26 0 1 "5V" 1 "25 ms" 1 "25 ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "-5V" 1>
  <.TR TR1 1 1040 460 0 79 0 0 "lin" 1 "0" 1 "100 ms" 1 "50000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <310 540 310 590 "" 0 0 0 "">
  <310 480 420 480 "Vin" 370 440 47 "">
  <620 480 730 480 "" 0 0 0 "">
  <730 480 730 590 "" 0 0 0 "">
  <690 590 730 590 "" 0 0 0 "">
  <480 480 510 480 "" 0 0 0 "">
  <510 480 560 480 "" 0 0 0 "">
  <510 480 510 570 "" 0 0 0 "">
  <510 570 620 570 "" 0 0 0 "">
  <310 650 310 690 "" 0 0 0 "">
  <310 690 310 710 "" 0 0 0 "">
  <310 690 620 690 "" 0 0 0 "">
  <620 610 620 690 "" 0 0 0 "">
  <730 590 730 590 "Vout" 760 560 0 "">
</Wires>
<Diagrams>
  <Rect 220 -91 1237 469 3 #c0c0c0 1 00 1 0 0.1 1 1 -17.2 5 10 1 -1 0.2 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 200 342 1247 372 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 870 630 9 #000000 0 "This particular opamp integrator needs negative voltages from the rectangular wave.">
</Paintings>
