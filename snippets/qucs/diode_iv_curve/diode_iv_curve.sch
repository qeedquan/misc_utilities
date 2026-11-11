<Qucs Schematic 24.2.1>
<Properties>
  <View=-384,-10,2180,1063,0.826446,0,0>
  <Grid=10,10,1>
  <DataSet=diode_iv_curve.dat>
  <DataDisplay=diode_iv_curve.dpl>
  <OpenDisplay=0>
  <Script=diode_iv_curve.m>
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
  <.TR TR1 1 120 380 0 79 0 0 "lin" 1 "0" 1 "10s" 1 "1000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <GND * 1 80 230 0 0 0 0>
  <Diode D1 1 170 160 -26 -103 0 2 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Vac V1 1 80 200 18 -26 0 1 "1 V" 1 "1 Hz" 0 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 350 220 0 0 0 0>
  <IProbe Pr1 1 280 160 -26 16 0 0>
</Components>
<Wires>
  <80 160 80 170 "" 0 0 0 "">
  <80 160 140 160 "Vin" 90 110 13 "">
  <350 160 350 220 "" 0 0 0 "">
  <310 160 350 160 "" 0 0 0 "">
  <200 160 250 160 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 640 894 660 294 3 #c0c0c0 1 00 1 -1 0.2 1 1 -6.29891 20 69.2881 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@tran.v(vin)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 570 538 790 228 3 #c0c0c0 1 00 1 0 1 10 1 -6.2986 20 69.2846 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 580 263 777 213 3 #c0c0c0 1 00 1 0 1 10 1 -7.39849 20 69.3846 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin)@time" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -90 600 12 #000000 0 "Shows the Diode IV curve using an AC signal.\nWhen the voltage hits the forward voltage for the diode, it triggers a spike in the current.">
</Paintings>
