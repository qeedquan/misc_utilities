<Qucs Schematic 24.2.1>
<Properties>
  <View=-568,-650,1413,467,0.793907,0,1>
  <Grid=10,10,1>
  <DataSet=capacitor_charging_discharging.dat>
  <DataDisplay=capacitor_charging_discharging.dpl>
  <OpenDisplay=0>
  <Script=x.m>
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
  <GND * 1 430 400 0 0 0 0>
  <C C1 1 430 190 -65 -25 0 1 "200 u" 1 "0" 1 "neutral" 0>
  <VProbe Pr2 1 550 310 -16 28 0 3>
  <VProbe Pr1 1 560 190 -16 28 0 3>
  <R R1 1 430 310 -48 -22 0 1 "3k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 210 290 0 0 0 0>
  <Vrect V1 1 210 200 18 -26 0 1 "5 V" 1 "1" 1 "1" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0 V" 1>
  <.TR TR1 1 750 150 0 79 0 0 "lin" 1 "0" 1 "8" 1 "1000" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <SpiceIC SpiceIC1 1 -90 -230 -9 18 0 0 "v(out)=0" 1>
</Components>
<Wires>
  <430 220 430 280 "" 0 0 0 "">
  <430 340 430 400 "" 0 0 0 "">
  <430 340 530 340 "" 0 0 0 "">
  <530 320 530 340 "" 0 0 0 "">
  <430 280 530 280 "" 0 0 0 "">
  <530 280 530 300 "" 0 0 0 "">
  <430 220 540 220 "" 0 0 0 "">
  <540 200 540 220 "" 0 0 0 "">
  <430 160 540 160 "" 0 0 0 "">
  <540 160 540 180 "" 0 0 0 "">
  <430 130 430 160 "" 0 0 0 "">
  <210 130 210 170 "" 0 0 0 "">
  <210 130 430 130 "Vin" 260 100 17 "">
  <210 230 210 290 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 70 46 779 456 3 #c0c0c0 1 00 1 0 0.5 8 0 0 1 7 1 -1 0.2 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(pr1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(pr2)@time" #ff0000 0 3 0 0 0>
	<"ngspice/tran.v(vin)@time" #ff00ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 60 -610 9 #000000 0 "Simulate capacitor charging and discharging over time.\nUse a rectangular voltage source that turns on and off at specified time interval.\nWhen the rectangular voltage output is low, the capacitor discharges, when it's high, the capacitor charges.\n\nNote that the voltage probe between the resistor have the inverse curve of the capacitor charge/discharge curve\n\nThe simulators by default assume steady state of 5V, so we don't see the capacitor charging curve, need to set the simulator to have an initial condition of V=0\nFor qucsator, we need to set the initial condition inside the capacitor to be V=0.\nFor ngspice, use the .IC section to set v(out)=0\n">
</Paintings>
