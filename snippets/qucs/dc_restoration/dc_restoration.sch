<Qucs Schematic 24.2.1>
<Properties>
  <View=-1538,3,2430,1447,1,1299,84>
  <Grid=10,10,1>
  <DataSet=dc_restoration.dat>
  <DataDisplay=dc_restoration.dpl>
  <OpenDisplay=0>
  <Script=dc_restoration.m>
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
  <.TR TR1 1 690 180 0 79 0 0 "lin" 1 "0" 1 "100ms" 1 "1000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vac V1 1 50 180 18 -26 0 1 "5V" 1 "500Hz" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 50 250 0 0 0 0>
  <C C1 1 180 140 -26 17 0 0 "5u" 1 "" 0 "neutral" 0>
  <GND * 1 260 300 0 0 0 0>
  <R R1 1 350 240 15 -26 0 1 "5K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 350 310 0 0 0 0>
  <Diode D_1N5408_1 1 260 240 -80 -26 0 3 "63n" 1 "1.7" 1 "53p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "14.1m" 0 "4.32u" 0 "0" 0 "0" 0 "1" 0 "1" 0 "1k" 0 "10u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
</Components>
<Wires>
  <50 210 50 250 "" 0 0 0 "">
  <50 140 50 150 "" 0 0 0 "">
  <50 140 150 140 "Vin" 100 100 20 "">
  <260 270 260 300 "" 0 0 0 "">
  <350 270 350 310 "" 0 0 0 "">
  <210 140 260 140 "" 0 0 0 "">
  <260 140 260 210 "" 0 0 0 "">
  <260 140 350 140 "" 0 0 0 "">
  <350 140 350 210 "" 0 0 0 "">
  <350 140 350 140 "Vout" 380 110 0 "">
</Wires>
<Diagrams>
  <Rect 410 778 937 418 3 #c0c0c0 1 00 1 0 0.0001 0.001 1 -0.631005 1 5.44339 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vin)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -30 360 9 #000000 0 "DC restoration centers the AC waveform near to the DC point (zero)\n">
</Paintings>
