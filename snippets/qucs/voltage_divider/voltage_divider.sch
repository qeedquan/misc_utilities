<Qucs Schematic 24.2.1>
<Properties>
  <View=-4,-110,1202,579,1.30479,0,0>
  <Grid=10,10,1>
  <DataSet=voltage_divider.dat>
  <DataDisplay=voltage_divider.dpl>
  <OpenDisplay=0>
  <Script=voltage_divider.m>
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
  <GND * 1 370 330 0 0 0 0>
  <R R2 1 460 270 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Vdc V1 1 290 220 18 -26 0 1 "1 V" 1>
  <R R1 1 460 170 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <.DC DC1 1 460 470 0 45 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
</Components>
<Wires>
  <370 310 370 330 "" 0 0 0 "">
  <370 310 460 310 "" 0 0 0 "">
  <460 300 460 310 "" 0 0 0 "">
  <290 310 370 310 "" 0 0 0 "">
  <290 250 290 310 "" 0 0 0 "">
  <290 140 290 190 "" 0 0 0 "">
  <290 140 460 140 "Test_Point" 380 110 61 "">
  <460 200 460 240 "Divider" 490 190 19 "">
</Wires>
<Diagrams>
  <Tab 620 320 300 200 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/v(divider)" #0000ff 0 3 1 0 0>
	<"ngspice/v(test_point)" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 290 -70 12 #000000 0 "Voltage Divider\nSplits the voltage in half at the output\n\nA node label can be used to serve as a voltage probe as shown in the circuit below\n\n">
</Paintings>
