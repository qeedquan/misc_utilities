<Qucs Schematic 24.2.1>
<Properties>
  <View=-1226,-367,2146,685,1,876,60>
  <Grid=10,10,1>
  <DataSet=diode_limiter.dat>
  <DataDisplay=diode_limiter.dpl>
  <OpenDisplay=0>
  <Script=diode_limiter.m>
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
  <Vac V1 1 -110 -10 18 -26 0 1 "5V" 1 "40" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <Diode D2 1 230 0 20 -31 0 3 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Diode D1 1 110 0 15 -26 0 1 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND * 1 -110 80 0 0 0 0>
  <IProbe Pr1 1 100 -100 -26 16 0 0>
  <R R1 1 20 -100 -26 15 0 0 "110" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.TR TR1 1 -150 200 0 79 0 0 "lin" 1 "0" 1 "100 ms" 1 "1000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <180 -100 180 -50 "" 0 0 0 "">
  <-110 20 -110 60 "" 0 0 0 "">
  <230 30 230 60 "" 0 0 0 "">
  <180 -50 230 -50 "" 0 0 0 "">
  <230 -50 230 -30 "" 0 0 0 "">
  <110 -50 180 -50 "" 0 0 0 "">
  <110 -50 110 -30 "" 0 0 0 "">
  <-110 60 110 60 "" 0 0 0 "">
  <110 60 230 60 "" 0 0 0 "">
  <110 30 110 60 "" 0 0 0 "">
  <-110 60 -110 80 "" 0 0 0 "">
  <130 -100 180 -100 "" 0 0 0 "">
  <50 -100 70 -100 "" 0 0 0 "">
  <-110 -100 -110 -40 "" 0 0 0 "">
  <-110 -100 -10 -100 "" 0 0 0 "">
  <180 -100 180 -100 "Vout" 210 -130 0 "">
  <-110 -100 -110 -100 "Vin" -70 -130 0 "">
</Wires>
<Diagrams>
  <Rect 430 -135 517 175 3 #c0c0c0 1 00 1 0 0.01 0.1 1 -6 5 5.99999 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vout)@time" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 410 222 565 292 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@time" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 10 140 9 #000000 0 "Diode limiters clamps the voltage at the output.">
</Paintings>