<Qucs Schematic 24.2.1>
<Properties>
  <View=-646,-602,1627,939,0.692187,0,0>
  <Grid=10,10,1>
  <DataSet=inverted_transistor_level_shifter.dat>
  <DataDisplay=inverted_transistor_level_shifter.dpl>
  <OpenDisplay=0>
  <Script=inverted_transistor_level_shifter.m>
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
  <R R2 1 270 350 -26 15 0 0 "1 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 140 490 0 0 0 0>
  <GND * 1 430 500 0 0 0 0>
  <_BJT T_2N2222_1 1 430 350 12 -32 0 0 "npn" 0 "1e-14" 0 "1" 0 "1" 0 "0.3" 0 "0" 0 "100" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "200" 0 "3" 0 "0" 0 "0" 0 "3" 0 "1" 0 "10" 0 "25e-12" 0 "0.75" 0 "0.33" 0 "8e-12" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "400e-12" 0 "3" 0 "0.0" 0 "2" 0 "100e-9" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <.TR TR1 1 710 90 0 79 0 0 "lin" 1 "0" 1 "10" 1 "2000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vrect V2 1 140 430 18 -26 0 1 "3.3 V" 1 "1" 1 "1" 1 "1 ms" 0 "1 ms" 0 "0 ns" 0 "0 V" 1>
  <R R1 1 270 180 -26 15 0 0 "4.7KOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr2 1 370 350 -26 16 0 0>
  <IProbe Pr1 1 380 180 -26 16 0 0>
  <Vdc V1 1 80 210 18 -26 0 1 "5 V" 1>
  <GND * 1 80 270 0 0 0 0>
</Components>
<Wires>
  <300 350 340 350 "" 0 0 0 "">
  <430 380 430 500 "" 0 0 0 "">
  <140 460 140 490 "" 0 0 0 "">
  <140 350 240 350 "Vin" 170 310 22 "">
  <140 350 140 400 "" 0 0 0 "">
  <430 180 430 320 "Vout" 460 220 76 "">
  <410 180 430 180 "" 0 0 0 "">
  <300 180 350 180 "" 0 0 0 "">
  <80 240 80 270 "" 0 0 0 "">
  <80 180 240 180 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect -14 73 587 373 3 #c0c0c0 1 00 1 0 1 10 0 0 1 10 1 -1 0.2 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #ff0000 0 3 0 0 0>
	<"ngspice/tran.v(vin)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 690 -18 430 212 3 #c0c0c0 1 00 1 0 1 10 1 -0.000106643 0.0005 0.00116625 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.i(pr2)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -140 590 12 #000000 0 "A level shifter is a circuit that can output a different voltage level than the voltage level it is controlled on, it "shifts" the voltage output.\nOne way to make it is to use a transistor combined with two voltage sources at different levels.\n\nIn this circuit, when the transistor is off, Vout will be at 5V (with no current), it will use the voltage at V1.\nWhen the transistor is on (V2 goes to 3.3V), the current will start flowing at that point and the voltage output will be 0 at Vout since it will be "pulled" ground\n\nWith this setup, the output is inverted, in the sense that if we apply a high to the control voltage, it outputs low, and high when the control voltage is low.\nThis is fine if the later stages note this fact, though we can fix the inversion by introducing another transistor level shifter stage.\n\nR1 here is usually called a "pull-up" resistor because it pulls up the output when Vout is floating (V2 is off)\nThe resistor values don't matter too much because we care about the voltage output rather than the current output,\nbut we can see if we change the resistor values, the current changes, and depending on the later stages, this could matter.\n\nThis type of circuit is usually used in I2C bus protocol.">
</Paintings>
