<Qucs Schematic 24.2.1>
<Properties>
  <View=-653,-242,1573,501,1.21,463,0>
  <Grid=10,10,1>
  <DataSet=dc_voltage_and_current_source.dat>
  <DataDisplay=dc_voltage_and_current_source.dpl>
  <OpenDisplay=0>
  <Script=dc_voltage_and_current_source.m>
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
  <R R1 1 420 180 -26 15 0 0 "5 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 520 250 15 -26 0 1 "10 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 520 310 0 0 0 0>
  <.DC DC1 1 750 320 0 46 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 100 310 0 0 0 0>
  <Vdc V1 1 100 210 18 -26 0 1 "1 V" 1>
  <IProbe Pr1 1 230 180 -26 -38 0 2>
  <GND * 1 810 180 0 0 0 1>
  <Idc I1 1 710 180 -26 -62 0 2 "1 A" 1>
  <IProbe Pr2 1 610 180 -26 -38 0 2>
</Components>
<Wires>
  <450 180 520 180 "" 0 0 0 "">
  <520 280 520 310 "" 0 0 0 "">
  <520 180 520 220 "" 0 0 0 "">
  <100 240 100 310 "" 0 0 0 "">
  <260 180 390 180 "Node_1" 320 120 62 "">
  <100 180 200 180 "" 0 0 0 "">
  <740 180 810 180 "" 0 0 0 "">
  <520 180 580 180 "Node_2" 470 120 20 "">
  <640 180 680 180 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Tab 200 54 397 194 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/v(node_1)" #0000ff 0 3 1 0 0>
	<"ngspice/v(node_2)" #0000ff 0 3 1 0 0>
	<"ngspice/i(pr1)" #0000ff 0 3 1 0 0>
	<"ngspice/i(pr2)" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
</Paintings>
