<Qucs Schematic 24.2.1>
<Properties>
  <View=-370,-440,1310,570,0.936575,0,60>
  <Grid=10,10,1>
  <DataSet=diode_half_wave_rectifier.dat>
  <DataDisplay=diode_half_wave_rectifier.dpl>
  <OpenDisplay=0>
  <Script=full_wave_rectifier.m>
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
  <Vac V1 1 120 270 18 -26 0 1 "5V" 1 "20" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 120 440 0 0 0 0>
  <GND * 1 680 440 0 0 0 0>
  <R R1 1 680 330 15 -26 0 1 "640" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Diode D3 1 400 200 -26 -79 0 2 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <.TR TR1 1 790 150 0 79 0 0 "lin" 1 "0" 1 "1" 1 "5000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <120 300 120 440 "" 0 0 0 "">
  <680 360 680 440 "" 0 0 0 "">
  <120 200 120 240 "" 0 0 0 "">
  <120 200 370 200 "Vin" 220 150 75 "">
  <680 200 680 300 "Vout" 710 200 36 "">
  <430 200 680 200 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 40 49 859 359 3 #c0c0c0 1 00 1 0 0.05 1 1 -6 2 6 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vin)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 140 -400 9 #000000 0 "A half-wave rectifier clips the negative parts of the AC to zero, so only the positive part is seen.\nIf we want the negative parts to be seen as a positive voltage, use a full wave rectifier.\nThere is alot of different circuit design for a half-wave/full-wave rectifier with tradeoff of simplicity, efficiency, etc">
</Paintings>
