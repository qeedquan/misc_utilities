<Qucs Schematic 24.2.1>
<Properties>
  <View=-160,-142,2143,744,1,190,0>
  <Grid=10,10,1>
  <DataSet=opamp_comparator.dat>
  <DataDisplay=opamp_comparator.dpl>
  <OpenDisplay=0>
  <Script=opamp_comparator.m>
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
  <GND * 1 300 200 0 0 0 0>
  <Vdc V2 1 300 320 18 -26 0 1 "3 V" 1>
  <GND * 1 300 380 0 0 0 0>
  <OpAmp OP1 1 480 140 -26 42 0 0 "1e6" 1 "10V" 1>
  <.TR TR1 1 680 -50 0 79 0 0 "lin" 1 "0" 1 "10" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vac V1 1 300 110 18 -26 0 1 "5 V" 1 "1 " 1 "0" 0 "0" 0 "0" 0 "0" 0>
</Components>
<Wires>
  <430 160 450 160 "" 0 0 0 "">
  <430 120 450 120 "" 0 0 0 "">
  <430 50 430 120 "" 0 0 0 "">
  <300 50 430 50 "Vin1" 390 10 71 "">
  <300 50 300 80 "" 0 0 0 "">
  <300 140 300 170 "" 0 0 0 "">
  <300 170 300 200 "" 0 0 0 "">
  <430 160 430 280 "" 0 0 0 "">
  <300 280 430 280 "Vin2" 380 230 67 "">
  <300 280 300 290 "" 0 0 0 "">
  <300 350 300 380 "" 0 0 0 "">
  <520 140 520 140 "Vout" 550 110 0 "">
</Wires>
<Diagrams>
  <Rect 840 595 580 445 3 #c0c0c0 1 00 1 0 1 10 1 -12 2 12 1 -1 0.2 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin1)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vin2)" #ff0000 0 3 0 0 0>
	<"ngspice/tran.v(vout)" #ff00ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 100 460 12 #000000 0 "An opamp can act like a comparator.\nIf the input on the positive side > negative side, then output +Vmax.\nIf the input on the positive side = negative side, then output 0.\nIf the input on the positive side < negative side, then output -Vmax.\nVmax the max voltage output that the op-amp supports.\n">
</Paintings>
