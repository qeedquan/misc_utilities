<Qucs Schematic 24.2.1>
<Properties>
  <View=-242,-99,1761,672,1.14985,322,0>
  <Grid=10,10,1>
  <DataSet=resistor_series_and_parallel.dat>
  <DataDisplay=resistor_series_and_parallel.dpl>
  <OpenDisplay=0>
  <Script=resistor_series_and_parallel.m>
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
  <Vdc V1 1 290 500 18 -26 0 1 "6" 1>
  <GND * 1 290 600 0 0 0 0>
  <R R1 1 410 310 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 610 510 15 -26 0 1 "12" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr1 1 670 310 -26 16 0 0>
  <R R6 1 1180 510 15 -26 0 1 "10" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 1300 310 -26 15 0 0 "2" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R7 1 1330 510 15 -26 0 1 "8" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R5 1 810 510 15 -26 0 1 "4" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 900 310 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr2 1 1090 310 -26 16 0 0>
  <.CUSTOMSIM CUSTOM2 1 260 -60 0 51 0 0 "display\nlisting\nlisting e\nprint all\n" 1 "V(out);V(in)" 0 "custom#ac1#.plot;custom#ac1#.print" 0>
  <.TR TR1 1 540 -70 0 75 0 0 "lin" 1 "0" 1 "1 ms" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.DC DC1 1 720 -50 0 46 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
</Components>
<Wires>
  <290 530 290 590 "" 0 0 0 "">
  <290 310 380 310 "" 0 0 0 "">
  <290 310 290 470 "" 0 0 0 "">
  <290 590 290 600 "" 0 0 0 "">
  <290 590 610 590 "" 0 0 0 "">
  <610 540 610 590 "" 0 0 0 "">
  <440 310 610 310 "" 0 0 0 "">
  <610 310 640 310 "" 0 0 0 "">
  <610 310 610 480 "" 0 0 0 "">
  <1180 540 1180 590 "" 0 0 0 "">
  <1180 310 1180 480 "" 0 0 0 "">
  <1180 310 1270 310 "" 0 0 0 "">
  <1180 590 1330 590 "" 0 0 0 "">
  <1330 540 1330 590 "" 0 0 0 "">
  <1330 310 1330 480 "" 0 0 0 "">
  <700 310 810 310 "" 0 0 0 "">
  <610 590 810 590 "" 0 0 0 "">
  <810 590 1180 590 "" 0 0 0 "">
  <810 540 810 590 "" 0 0 0 "">
  <810 310 810 480 "" 0 0 0 "">
  <810 310 870 310 "" 0 0 0 "">
  <1120 310 1180 310 "" 0 0 0 "">
  <930 310 1060 310 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 420 170 9 #000000 0 "A resistor network of series and parallel resistances.\nThe nutmeg script is passed to the spice simulator to run so we can add spice code to run for more information.">
</Paintings>
