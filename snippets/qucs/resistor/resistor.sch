<Qucs Schematic 24.2.1>
<Properties>
  <View=-180,-427,1327,491,1.04413,0,0>
  <Grid=10,10,1>
  <DataSet=resistor.dat>
  <DataDisplay=resistor.dpl>
  <OpenDisplay=0>
  <Script=resistor.m>
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
  <Vdc V1 1 270 180 18 -26 0 1 "5V" 1>
  <GND * 1 270 250 0 0 0 0>
  <.DC DC1 1 470 380 0 47 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <R R1 1 480 150 -26 15 0 0 "2 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 760 260 0 0 0 0>
  <R R2 1 620 150 -26 15 0 0 "1.5 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <VProbe Pr1 1 470 90 28 -31 0 0>
  <VProbe Pr2 1 610 90 28 -31 0 0>
  <IProbe Pr3 1 370 150 -26 16 0 0>
  <IProbe Pr4 1 550 150 -26 16 0 0>
  <IProbe Pr5 1 700 150 -26 16 0 0>
</Components>
<Wires>
  <270 210 270 250 "" 0 0 0 "">
  <760 260 770 260 "" 0 0 0 "">
  <770 150 770 260 "" 0 0 0 "">
  <650 150 670 150 "" 0 0 0 "">
  <270 150 340 150 "" 0 0 0 "">
  <510 150 520 150 "" 0 0 0 "">
  <450 110 450 150 "" 0 0 0 "">
  <450 110 460 110 "" 0 0 0 "">
  <480 110 510 110 "" 0 0 0 "">
  <510 110 510 150 "" 0 0 0 "">
  <590 110 600 110 "" 0 0 0 "">
  <620 110 650 110 "" 0 0 0 "">
  <590 110 590 150 "" 0 0 0 "">
  <650 110 650 150 "" 0 0 0 "">
  <400 150 450 150 "" 0 0 0 "">
  <580 150 590 150 "" 0 0 0 "">
  <730 150 770 150 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Tab 270 22 386 202 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/i(pr3)" #0000ff 0 3 1 0 0>
	<"ngspice/i(pr4)" #0000ff 0 3 1 0 0>
	<"ngspice/i(pr5)" #0000ff 0 3 1 0 0>
	<"ngspice/v(pr1)" #0000ff 0 3 1 0 0>
	<"ngspice/v(pr2)" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 160 -330 12 #000000 0 "Series Resistor Circuit\nThe current is the same everywhere.\nThe voltage drops between resistors add up to the original voltage source\n\nVoltage Probe has to be between components, cannot place them on the same wire since that is a short circuit\nCurrent Probe can be placed on the path that the component is traveling">
</Paintings>
