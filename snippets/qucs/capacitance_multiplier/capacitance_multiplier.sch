<Qucs Schematic 24.2.1>
<Properties>
  <View=-305,-85,1628,934,1,90,120>
  <Grid=10,10,1>
  <DataSet=capacitance_multiplier.dat>
  <DataDisplay=capacitance_multiplier.dpl>
  <OpenDisplay=0>
  <Script=capacitance_multiplier.m>
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
  <GND * 1 230 580 0 0 0 0>
  <GND * 1 600 560 0 0 0 0>
  <C C2 1 380 480 -26 17 0 0 "10u" 1 "" 0 "neutral" 0>
  <R R2 1 520 480 -26 15 0 0 "1K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vrect V2 1 230 510 18 -26 0 1 "5V" 1 "33.33 ms" 1 "33.33 ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0V" 1>
  <VProbe Pr1 1 380 410 28 -31 0 0>
  <Vrect V1 1 180 140 18 -26 0 1 "5V" 1 "33.33 ms" 1 "33.33 ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0 V" 1>
  <GND * 1 180 210 0 0 0 0>
  <R R1 1 340 100 -26 15 0 0 "100k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <OpAmp OP1 1 580 80 -26 -74 1 0 "1e6" 1 "15 V" 0>
  <R R3 1 420 -30 -26 15 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 480 290 0 0 0 0>
  <VProbe Pr2 1 420 230 -31 28 1 3>
  <.TR TR1 1 940 600 0 77 0 0 "lin" 1 "0" 1 "500 ms" 1 "50000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <C C1 1 480 230 17 -26 0 1 "100n" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <230 540 230 580 "" 0 0 0 "">
  <230 480 350 480 "" 0 0 0 "">
  <600 480 600 560 "" 0 0 0 "">
  <550 480 600 480 "" 0 0 0 "">
  <410 480 490 480 "" 0 0 0 "">
  <390 430 410 430 "" 0 0 0 "">
  <410 430 410 480 "" 0 0 0 "">
  <350 430 370 430 "" 0 0 0 "">
  <350 430 350 480 "" 0 0 0 "">
  <180 170 180 210 "" 0 0 0 "">
  <180 100 180 110 "" 0 0 0 "">
  <180 100 290 100 "" 0 0 0 "">
  <290 100 310 100 "" 0 0 0 "">
  <290 -30 290 100 "" 0 0 0 "">
  <290 -30 390 -30 "" 0 0 0 "">
  <450 -30 630 -30 "" 0 0 0 "">
  <620 80 630 80 "" 0 0 0 "">
  <490 60 550 60 "" 0 0 0 "">
  <490 -10 490 60 "" 0 0 0 "">
  <630 -30 630 -10 "" 0 0 0 "">
  <630 -10 630 80 "" 0 0 0 "">
  <490 -10 630 -10 "" 0 0 0 "">
  <370 100 480 100 "" 0 0 0 "">
  <480 100 550 100 "" 0 0 0 "">
  <480 100 480 200 "" 0 0 0 "">
  <480 260 480 290 "" 0 0 0 "">
  <440 260 480 260 "" 0 0 0 "">
  <440 240 440 260 "" 0 0 0 "">
  <440 200 480 200 "" 0 0 0 "">
  <440 200 440 220 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 740 499 454 249 3 #c0c0c0 1 00 1 0 0.05 0.5 1 -0.482768 1 5.31044 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(pr1)@time" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 740 197 473 237 3 #c0c0c0 1 00 1 -1 0.5 1 1 -1 1 1 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(pr1)@time" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -30 700 9 #000000 0 "A capacitance multiplier is designed to make a capacitor function like a much larger capacitor. This can be achieved in at least two ways.\nAn active circuit, using a device such as a transistor or operational amplifier\nA passive circuit, using autotransformers. These are typically used for calibration standards. The General Radio / IET labs 1417 is one such example.\nCapacitor multipliers make low-frequency filters and long-duration timing circuits possible that would be impractical with actual capacitors.\nAnother application is in DC power supplies where very low ripple voltage (under load) is of paramount importance, such as in class-A amplifiers.\n\nHere, the capacitance of capacitor C1 is multiplied by the ratio of resistances: C = C1 * R1 / R2 at the Vi node.\n\nThis simulation shows that we can use a smaller capacitor to make it look like a larger one">
</Paintings>
