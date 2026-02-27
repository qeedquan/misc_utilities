<Qucs Schematic 24.2.1>
<Properties>
  <View=227,-67,1241,440,1.77156,0,11>
  <Grid=10,10,1>
  <DataSet=inductor.dat>
  <DataDisplay=inductor.dpl>
  <OpenDisplay=0>
  <Script=inductor.m>
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
  <L L1 1 430 240 10 -26 1 3 "1H" 1 "" 0>
  <GND * 1 360 300 0 0 0 0>
  <.TR TR1 1 610 280 0 75 0 0 "lin" 1 "0" 1 "1" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R R1 1 370 200 -26 15 0 0 "180" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vac V1 1 280 240 18 -26 0 1 "5V" 1 "20" 0 "0" 0 "0" 0 "0" 0 "0" 0>
</Components>
<Wires>
  <280 200 280 210 "" 0 0 0 "">
  <280 200 340 200 "Vin" 320 160 20 "">
  <400 200 430 200 "Vout" 450 170 20 "">
  <430 200 430 210 "" 0 0 0 "">
  <280 270 280 300 "" 0 0 0 "">
  <280 300 360 300 "" 0 0 0 "">
  <430 270 430 300 "" 0 0 0 "">
  <360 300 430 300 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 560 223 405 243 3 #c0c0c0 1 00 1 0 0.2 1 1 -5.99999 5 6 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vout)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
