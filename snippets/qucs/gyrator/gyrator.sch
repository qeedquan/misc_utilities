<Qucs Schematic 24.2.1>
<Properties>
  <View=-1137,-43,2070,1029,0.826446,636,0>
  <Grid=10,10,1>
  <DataSet=gyrator.dat>
  <DataDisplay=gyrator.dpl>
  <OpenDisplay=0>
  <Script=gyrator.m>
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
  <GND * 1 -120 260 0 0 0 0>
  <C C1 1 20 180 -26 17 0 0 "250n" 1 "" 0 "neutral" 0>
  <R R1 1 20 110 -26 15 0 0 "1K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 160 260 15 -26 0 1 "20K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 160 310 0 0 0 0>
  <Vrect V1 1 -120 190 18 -26 0 1 "5V" 1 "50 ms" 1 "50 ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0 V" 1>
  <OpAmp OP1 1 260 160 -26 -74 1 0 "1e6" 1 "15 V" 0>
  <GND * 1 -30 540 0 0 0 0>
  <L L1 1 280 490 -37 -26 0 3 "5H" 1 "" 0>
  <GND * 1 280 550 0 0 0 0>
  <Vrect V2 1 -30 490 18 -26 0 1 "5 V" 1 "50 ms" 1 "50 ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0 V" 1>
  <R R2 1 90 460 -26 15 0 0 "1K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr2 1 190 460 -26 16 0 0>
  <IProbe Pr1 1 300 30 -26 16 0 0>
  <.TR TR1 1 -60 680 0 79 0 0 "lin" 1 "0" 1 "1" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <-120 220 -120 260 "" 0 0 0 "">
  <-120 110 -120 160 "" 0 0 0 "">
  <-120 110 -40 110 "Vin1" -50 80 36 "">
  <-40 110 -10 110 "" 0 0 0 "">
  <-40 110 -40 180 "" 0 0 0 "">
  <-40 180 -10 180 "" 0 0 0 "">
  <50 180 160 180 "" 0 0 0 "">
  <160 180 160 230 "" 0 0 0 "">
  <160 290 160 310 "" 0 0 0 "">
  <140 30 140 110 "" 0 0 0 "">
  <140 30 270 30 "" 0 0 0 "">
  <50 110 140 110 "" 0 0 0 "">
  <360 30 360 160 "" 0 0 0 "">
  <300 160 360 160 "" 0 0 0 "">
  <160 180 230 180 "" 0 0 0 "">
  <140 110 140 140 "" 0 0 0 "">
  <140 140 230 140 "" 0 0 0 "">
  <-30 520 -30 540 "" 0 0 0 "">
  <280 520 280 550 "" 0 0 0 "">
  <-30 460 60 460 "Vin2" 20 410 20 "">
  <220 460 280 460 "" 0 0 0 "">
  <120 460 160 460 "" 0 0 0 "">
  <330 30 360 30 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 960 233 404 253 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.2 1.1 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin2)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 500 233 404 253 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.5 2 6 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin1)@time" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 610 460 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.000499991 0.002 0.006 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@time" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 1050 460 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.000499991 0.002 0.006 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr2)@time" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 440 630 9 #000000 0 "A gyrator was originally hypothesized to be a 5th linear element after resistor, capacitor, inductor and ideal transformer.\nUnlike the four conventional elements, the gyrator is non-reciprocal.\nGyrators permit network realizations of two-(or-more)-port devices which cannot be realized with just the four conventional elements.\nIn particular, gyrators make possible network realizations of isolators and circulators.\nGyrators do not however change the range of one-port devices that can be realized.\n\nWe can simulate a gyrator using a opamp combined with RLC circuits (at low frequencies).\nIn this case, we simulate an inductor using resistors and capacitors.\nAn inductor can be replaced by a much smaller assembly containing a capacitor, operational amplifiers or transistors, and resistors.\nThis is especially useful in integrated circuit technology.\n\n\n\n">
</Paintings>
