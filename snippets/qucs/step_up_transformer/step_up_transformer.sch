<Qucs Schematic 24.2.1>
<Properties>
  <View=-1090,-231,2200,728,1,886,0>
  <Grid=10,10,1>
  <DataSet=step_up_transformer.dat>
  <DataDisplay=step_up_transformer.dpl>
  <OpenDisplay=0>
  <Script=step_up_transformer.m>
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
  <R R2 1 600 310 15 -26 0 1 "2k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 210 390 0 0 0 0>
  <GND * 1 600 390 0 0 0 0>
  <Vac V1 1 210 310 18 -26 0 1 "10" 1 "60" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <R R1 1 320 280 -26 15 0 0 "100m" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr1 1 390 280 -26 16 0 0>
  <IProbe Pr2 1 520 280 -26 16 0 0>
  <.TR TR1 1 -40 190 0 79 0 0 "lin" 1 "0" 1 "1" 1 "2000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <MUT Tr1 1 450 310 -29 38 0 0 "1" 1 "100" 1 "1" 1>
</Components>
<Wires>
  <210 280 290 280 "Vin" 260 240 35 "">
  <210 340 210 390 "" 0 0 0 "">
  <600 340 600 390 "" 0 0 0 "">
  <210 340 420 340 "" 0 0 0 "">
  <480 340 600 340 "" 0 0 0 "">
  <480 280 490 280 "" 0 0 0 "">
  <350 280 360 280 "" 0 0 0 "">
  <550 280 600 280 "Vout" 580 240 11 "">
</Wires>
<Diagrams>
  <Rect 830 454 366 204 3 #c0c0c0 1 00 1 0 0.2 1 1 -119.429 100 119.379 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 820 206 379 196 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 520 110 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.0597143 0.05 0.0596894 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr2)@time" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 210 110 240 160 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@time" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 90 520 9 #000000 0 "A step-up transformer makes the voltage larger, the ratio larger is defined by the winding ratio.\nThe winding ratio is defined as N1/N2 where N1 is the inductance of the primary winding and N2 is the inductance of the secondary winding.\nIn this case, we have 1:10 the voltage would be step-up by 10 times.\n\nSince transformers are passive components made of 2 inductors wound together, it can't increase the power, so by increasing voltage by a factor, the current decreases by that same factor.\n\nThe factor that also affects the output is the coupling factor.\nThe coupling factor is a value between 0 and 1. 1 expresses perfect coupling, i.e. all flux generated penetrates the receiver coil.\n0 expresses a system, where transmitter and receiver coils are independent of each other.\nThe coupling factor is determined by the distance between the inductors and their relative size.\nA coupling factor of 1 gives the expected ratio output, but lower values distort the wave output.">
</Paintings>
