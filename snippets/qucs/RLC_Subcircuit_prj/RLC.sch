<Qucs Schematic 24.2.1>
<Properties>
  <View=192,143,747,460,2.83935,0,0>
  <Grid=10,10,1>
  <DataSet=RLC.dat>
  <DataDisplay=RLC.dpl>
  <OpenDisplay=0>
  <Script=RLC.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -70 0 30 0 #000080 2 1>
  <.PortSym 70 0 2 180>
  <.PortSym -70 0 1 0>
  <Line 40 0 30 0 #000080 2 1>
  <Line -40 -20 0 40 #000080 2 1>
  <Line -40 20 80 0 #000080 2 1>
  <Line -40 -20 80 0 #000080 2 1>
  <Line 40 -20 0 40 #000080 2 1>
  <.ID -30 34 RLC "1=Ls=1 nH==" "1=Cs=1 pF==" "1=Rs=50 Ohm==">
  <Text -30 -10 10 #000000 0 "I">
  <Text 20 -10 10 #000000 0 "O">
</Symbol>
<Components>
  <Port P1 1 260 250 -23 12 0 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <L L1 1 360 250 -26 10 0 0 "Ls" 1 "" 0>
  <GND * 1 550 340 0 0 0 0>
  <Port P2 1 650 250 0 14 0 2 "2" 1 "analog" 0 "v" 0 "" 0>
  <C C1 1 460 250 -26 17 0 0 "Cs" 1 "" 0 "neutral" 0>
  <R R1 1 550 280 15 -26 0 1 "Rs" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
</Components>
<Wires>
  <260 250 330 250 "" 0 0 0 "">
  <390 250 430 250 "" 0 0 0 "">
  <490 250 550 250 "" 0 0 0 "">
  <550 310 550 340 "" 0 0 0 "">
  <550 250 650 250 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
