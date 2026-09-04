<Qucs Schematic 24.2.1>
<Properties>
  <View=-454,-670,2346,900,0.683013,139,112>
  <Grid=10,10,1>
  <DataSet=rectangular_voltage_source_parameters_for_triangle_wave.dat>
  <DataDisplay=rectangular_voltage_source_parameters_for_triangle_wave.dpl>
  <OpenDisplay=0>
  <Script=rectangular_voltage_source_parameters_for_triangle_wave.m>
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
  <R R1 1 420 -470 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 290 -370 0 0 0 0>
  <GND * 1 470 -370 0 0 0 0>
  <Vrect V1 1 290 -440 18 -26 0 1 "1 V" 1 "1 ns" 1 "1 ns" 1 "1 ms" 1 "1 ns" 1 "0 ns" 0 "0 V" 1>
  <R R2 1 390 -60 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 260 40 0 0 0 0>
  <GND * 1 440 40 0 0 0 0>
  <Vrect V2 1 260 -30 18 -26 0 1 "1 V" 1 "1 ns" 1 "1 ns" 1 "1 ms" 1 "1 ms" 1 "0 ns" 0 "0 V" 1>
  <R R3 1 400 280 -26 15 0 0 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 270 380 0 0 0 0>
  <GND * 1 450 380 0 0 0 0>
  <Vrect V3 1 270 310 18 -26 0 1 "1 V" 1 "1 ns" 1 "1 ns" 1 "1 ns" 1 "1 ms" 1 "0 ns" 0 "0 V" 1>
  <.TR TR1 1 750 600 0 79 0 0 "lin" 1 "0" 1 "10ms" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <290 -470 390 -470 "Vin1" 360 -500 38 "">
  <290 -410 290 -370 "" 0 0 0 "">
  <450 -470 470 -470 "" 0 0 0 "">
  <470 -470 470 -370 "" 0 0 0 "">
  <260 -60 360 -60 "Vin2" 330 -90 38 "">
  <260 0 260 40 "" 0 0 0 "">
  <420 -60 440 -60 "" 0 0 0 "">
  <440 -60 440 40 "" 0 0 0 "">
  <270 280 370 280 "Vin3" 340 250 38 "">
  <270 340 270 380 "" 0 0 0 "">
  <430 280 450 280 "" 0 0 0 "">
  <450 280 450 380 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 560 -272 679 278 3 #c0c0c0 1 00 1 0 0.001 0.01 1 -0.1 0.2 1.1 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin1)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 560 138 679 278 3 #c0c0c0 1 00 1 0 0.001 0.01 1 -0.1 0.2 1.1 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin2)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 570 508 679 278 3 #c0c0c0 1 00 1 0 0.001 0.01 1 -0.1 0.2 1.1 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin3)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 130 600 9 #000000 0 "We can generate a triangle wave by adjusting the parameters for the rectangle wave:\nTL=TH\nTr and Tf can vary to make different looking triangle waves\nTr < Tf for a left sloped triangle\nTr = Tf for a symmetric triangle\nTf > Tr for right sloped triangle">
</Paintings>
