<Qucs Schematic 24.2.1>
<Properties>
  <View=-48,-120,1573,946,1,0,0>
  <Grid=10,10,1>
  <DataSet=current_to_voltage_converte.dat>
  <DataDisplay=current_to_voltage_converte.dpl>
  <OpenDisplay=0>
  <Script=current_to_voltage_converte.m>
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
  <Lib OP1 1 770 190 -20 -39 1 0 "Ideal" 0 "OpAmp" 0 "1E6" 0 "106" 0 "75" 0 "14" 0 "-14" 0>
  <GND * 1 740 260 0 0 0 0>
  <R R1 1 720 110 -26 15 0 0 "1 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vdc V1 1 390 210 18 -26 0 1 "10 V" 1>
  <GND * 1 390 280 0 0 0 0>
  <IProbe Pr1 1 480 180 -26 16 0 0>
  <.TR TR1 1 1000 100 0 75 0 0 "lin" 1 "0" 1 "1 ms" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Idc I1 1 570 180 -26 -50 0 2 "5 mA" 1>
</Components>
<Wires>
  <740 200 740 260 "" 0 0 0 "">
  <800 110 800 190 "" 0 0 0 "">
  <750 110 800 110 "" 0 0 0 "">
  <650 110 690 110 "" 0 0 0 "">
  <650 180 740 180 "" 0 0 0 "">
  <650 110 650 180 "" 0 0 0 "">
  <600 180 650 180 "" 0 0 0 "">
  <390 180 450 180 "" 0 0 0 "">
  <390 240 390 280 "" 0 0 0 "">
  <510 180 540 180 "" 0 0 0 "">
  <800 190 800 190 "Vout" 830 160 0 "">
</Wires>
<Diagrams>
  <Rect 50 828 641 488 3 #c0c0c0 1 00 1 0 0.0001 0.001 1 -0.262199 0.2 2.20019 1 -1 0.2 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 820 840 594 500 3 #c0c0c0 1 00 1 0 0.0001 0.001 1 -0.01 0.001 0 1 -1 0.2 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 40 10 9 #000000 0 "A current to voltage circuit outputs a voltage that is proportional to the value of the current:\nVout = I*scale_factor\nSince this is a simulation, the voltage will match the current at any value, but in reality there is a limit of how much the voltage output can match the current">
</Paintings>
