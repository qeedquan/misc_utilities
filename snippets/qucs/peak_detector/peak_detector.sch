<Qucs Schematic 24.2.1>
<Properties>
  <View=-1587,-839,3256,1285,1,1829,567>
  <Grid=10,10,1>
  <DataSet=peak_detector.dat>
  <DataDisplay=peak_detector.dpl>
  <OpenDisplay=0>
  <Script=peak_detector.m>
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
  <GND * 1 350 400 0 0 0 0>
  <C C1 1 770 350 17 -26 0 1 "10u" 1 "" 0 "neutral" 0>
  <GND * 1 770 410 0 0 0 0>
  <OpAmp OP1 1 550 220 -26 42 0 0 "1e6" 1 "15 V" 0>
  <OpAmp OP2 1 910 240 -26 42 0 0 "1e6" 1 "15 V" 0>
  <Diode D_1N5408_1 1 700 220 -26 -61 0 2 "63n" 1 "1.7" 1 "53p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "14.1m" 0 "4.32u" 0 "0" 0 "0" 0 "1" 0 "1" 0 "1k" 0 "10u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <GND * 1 350 700 0 0 0 0>
  <Vac V3 1 350 650 18 -26 0 1 "5" 1 "40" 0 "0" 0 "0" 0 "0" 0 "0" 0>
  <Vac V4 1 350 550 18 -26 0 1 "3" 1 "110" 0 "0" 0 "0" 0 "0" 0 "0" 0>
  <C C2 1 770 650 17 -26 0 1 "10u" 1 "" 0 "neutral" 0>
  <GND * 1 770 710 0 0 0 0>
  <OpAmp OP3 1 550 520 -26 42 0 0 "1e6" 1 "15 V" 0>
  <OpAmp OP4 1 910 540 -26 42 0 0 "1e6" 1 "15 V" 0>
  <Diode D_1N5408_2 1 700 520 -26 13 0 0 "63n" 1 "1.7" 1 "53p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "14.1m" 0 "4.32u" 0 "0" 0 "0" 0 "1" 0 "1" 0 "1k" 0 "10u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <Vac V1 1 350 250 18 -26 0 1 "3" 1 "110" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <Vac V2 1 350 350 18 -26 0 1 "5" 1 "40" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <.TR TR1 1 1290 300 0 84 0 0 "lin" 1 "0" 1 "200ms" 1 "50000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <350 380 350 400 "" 0 0 0 "">
  <350 280 350 320 "" 0 0 0 "">
  <510 320 770 320 "" 0 0 0 "">
  <770 220 770 320 "" 0 0 0 "">
  <770 380 770 410 "" 0 0 0 "">
  <770 220 880 220 "" 0 0 0 "">
  <510 240 510 320 "" 0 0 0 "">
  <510 240 520 240 "" 0 0 0 "">
  <350 200 350 220 "" 0 0 0 "">
  <350 200 520 200 "Vin1" 440 170 57 "">
  <880 260 880 320 "" 0 0 0 "">
  <880 320 1040 320 "" 0 0 0 "">
  <1040 240 1040 320 "" 0 0 0 "">
  <950 240 1040 240 "" 0 0 0 "">
  <590 220 670 220 "" 0 0 0 "">
  <730 220 770 220 "" 0 0 0 "">
  <350 680 350 700 "" 0 0 0 "">
  <350 580 350 620 "" 0 0 0 "">
  <510 620 770 620 "" 0 0 0 "">
  <770 520 770 620 "" 0 0 0 "">
  <770 680 770 710 "" 0 0 0 "">
  <770 520 880 520 "" 0 0 0 "">
  <510 540 510 620 "" 0 0 0 "">
  <510 540 520 540 "" 0 0 0 "">
  <350 500 350 520 "" 0 0 0 "">
  <350 500 520 500 "Vin2" 440 470 57 "">
  <880 560 880 620 "" 0 0 0 "">
  <880 620 1040 620 "" 0 0 0 "">
  <1040 540 1040 620 "" 0 0 0 "">
  <950 540 1040 540 "" 0 0 0 "">
  <590 520 670 520 "" 0 0 0 "">
  <730 520 770 520 "" 0 0 0 "">
  <1040 240 1040 240 "Vout1" 1070 210 0 "">
  <1040 540 1040 540 "Vout2" 1070 510 0 "">
</Wires>
<Diagrams>
  <Rect 600 71 522 261 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vout1)@time" #ff0000 0 3 0 0 0>
	<"ngspice/tran.v(vout2)@time" #ff00ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 1140 510 9 #000000 0 "Peak detector outputs the current seen max voltage from the input waveform.\nIf we reverse the diode, we can make it a minimum detector\n\nThe waveform in this setting is 5*sin(40*t) + 3*sin(110*t).\nWe can verify the max is working by just finding the derivative of the function, setting it to zero and evaluating at those points.\n\nThe convergence for this circuit needs a very small timestep.">
</Paintings>
