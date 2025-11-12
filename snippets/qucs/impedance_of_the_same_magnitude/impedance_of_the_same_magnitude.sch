<Qucs Schematic 24.2.1>
<Properties>
  <View=-131,16,1684,671,1.4641,357,60>
  <Grid=10,10,1>
  <DataSet=impedance_of_the_same_magnitude.dat>
  <DataDisplay=impedance_of_the_same_magnitude.dpl>
  <OpenDisplay=0>
  <Script=impedance_of_the_same_magnitude.m>
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
  <Vac V1 1 310 180 18 -26 0 1 "5V" 1 "80" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <R R1 1 420 140 -26 15 0 0 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 600 410 0 0 0 0>
  <GND * 1 310 230 0 0 0 0>
  <L L1 1 610 190 -53 -26 0 3 "344.6m" 1 "" 0>
  <GND * 1 610 230 0 0 0 0>
  <Vac V2 1 310 340 18 -26 0 1 "5V" 1 "80" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 310 400 0 0 0 0>
  <R R2 1 430 310 -26 15 0 0 "200" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr1 1 530 140 -26 16 0 0>
  <IProbe Pr2 1 530 310 -26 16 0 0>
  <IProbe Pr3 1 560 500 -26 16 0 0>
  <.TR TR1 1 790 290 0 75 0 0 "lin" 1 "0" 1 "100ms" 1 "5000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vac V3 1 320 530 18 -26 0 1 "5V" 1 "80" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 320 600 0 0 0 0>
  <GND * 1 590 600 0 0 0 0>
  <R R3 1 440 500 -26 15 0 0 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 590 550 17 -26 0 1 "11.5u" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <310 210 310 230 "" 0 0 0 "">
  <310 140 310 150 "" 0 0 0 "">
  <310 140 390 140 "" 0 0 0 "">
  <450 140 500 140 "" 0 0 0 "">
  <610 140 610 160 "" 0 0 0 "">
  <610 220 610 230 "" 0 0 0 "">
  <600 310 600 410 "" 0 0 0 "">
  <460 310 500 310 "" 0 0 0 "">
  <310 370 310 400 "" 0 0 0 "">
  <310 310 400 310 "" 0 0 0 "">
  <560 140 610 140 "" 0 0 0 "">
  <560 310 600 310 "" 0 0 0 "">
  <580 500 590 500 "" 0 0 0 "">
  <320 560 320 600 "" 0 0 0 "">
  <470 500 530 500 "" 0 0 0 "">
  <320 500 410 500 "" 0 0 0 "">
  <590 500 590 520 "" 0 0 0 "">
  <590 580 590 600 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 710 209 410 189 3 #c0c0c0 1 00 1 0 0.02 0.1 1 -0.0300706 0.02 0.0300322 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.i(pr2)@time" #ff0000 0 3 0 0 0>
	<"ngspice/tran.i(pr3)@time" #ff00ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 650 460 9 #000000 0 "Various RLC circuit configuration that gives the same magnitude at steady state.\nThis displays the phase shifts due to adding a capacitor or a inductor component">
</Paintings>
