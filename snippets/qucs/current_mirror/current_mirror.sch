<Qucs Schematic 24.2.1>
<Properties>
  <View=-723,-90,1580,890,1.21,741,0>
  <Grid=10,10,1>
  <DataSet=current_mirror.dat>
  <DataDisplay=current_mirror.dpl>
  <OpenDisplay=0>
  <Script=current_mirror.m>
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
  <R R1 1 360 190 15 -26 0 1 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 520 190 -49 -26 0 3 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vdc V1 1 230 160 -26 18 0 0 "5 V" 1>
  <GND * 1 180 160 0 0 0 3>
  <_BJT BC860C_1 1 360 300 -62 -26 0 2 "pnp" 0 "336.7f" 0 "1" 0 "1" 0 "0.1165" 0 "9.677" 0 "29.03" 0 "0" 0 "1.648p" 0 "2.405" 0 "196p" 0 "1.815" 0 "462.2" 0 "7.16" 0 "0" 0 "0" 0 "2.791" 0 "0" 0 "0" 0 "8.397p" 0 "0.5" 0 "0.3865" 0 "11.51p" 0 "0.5" 0 "0.3626" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "605.4p" 0 "41.48" 0 "10" 0 "3.088" 0 "10n" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <_BJT BC860C_2 1 520 300 6 -26 1 0 "pnp" 0 "336.7f" 0 "1" 0 "1" 0 "0.1165" 0 "9.677" 0 "29.03" 0 "0" 0 "1.648p" 0 "2.405" 0 "196p" 0 "1.815" 0 "462.2" 0 "7.16" 0 "0" 0 "0" 0 "2.791" 0 "0" 0 "0" 0 "8.397p" 0 "0.5" 0 "0.3865" 0 "11.51p" 0 "0.5" 0 "0.3626" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "605.4p" 0 "41.48" 0 "10" 0 "3.088" 0 "10n" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <IProbe Pr1 1 300 400 -32 -26 0 3>
  <IProbe Pr2 1 520 400 -32 -26 0 3>
  <GND * 1 520 570 0 0 0 0>
  <GND * 1 300 570 0 0 0 0>
  <.DC DC1 1 680 180 0 47 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <R R4 1 300 500 15 -26 0 1 "10" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R6 1 520 500 15 -26 0 1 "1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <180 160 200 160 "" 0 0 0 "">
  <260 160 360 160 "" 0 0 0 "">
  <360 160 520 160 "" 0 0 0 "">
  <360 220 360 270 "" 0 0 0 "">
  <520 220 520 270 "" 0 0 0 "">
  <390 300 430 300 "" 0 0 0 "">
  <430 300 490 300 "" 0 0 0 "">
  <430 300 430 330 "" 0 0 0 "">
  <360 330 430 330 "" 0 0 0 "">
  <300 330 360 330 "" 0 0 0 "">
  <300 330 300 370 "" 0 0 0 "">
  <520 330 520 370 "" 0 0 0 "">
  <300 430 300 470 "" 0 0 0 "">
  <520 430 520 470 "" 0 0 0 "">
  <520 530 520 570 "" 0 0 0 "">
  <300 530 300 570 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Tab 650 530 300 200 3 #c0c0c0 1 00 1 0 1 1 1 0 1 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 1 0 0>
	<"ngspice/i(pr2)" #0000ff 0 3 0 0 0>
  </Tab>
</Diagrams>
<Paintings>
  <Text 250 20 9 #000000 0 "A current mirror circuit mirrors the current output at two junctions.\nThe resistors asymmetric values in this circuit is very important:\nR1 > R2\nR4 > R6\nThe two current output will be very different if this is not the case.">
</Paintings>