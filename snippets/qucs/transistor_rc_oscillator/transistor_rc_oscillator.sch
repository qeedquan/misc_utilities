<Qucs Schematic 24.2.1>
<Properties>
  <View=-310,-330,1540,783,0.850288,0,60>
  <Grid=10,10,1>
  <DataSet=transistor_rc_oscillator.dat>
  <DataDisplay=transistor_rc_oscillator.dpl>
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
  <Vdc V1 1 620 160 18 -26 0 1 "10 V" 1>
  <GND * 1 620 240 0 0 0 0>
  <GND * 1 230 430 0 0 0 0>
  <GND * 1 500 430 0 0 0 0>
  <R R1 1 200 160 15 -26 0 1 "220" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 290 160 15 -26 0 1 "22k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R3 1 400 170 15 -26 0 1 "22k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R4 1 500 180 15 -26 0 1 "220" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <C C2 1 430 270 -26 -49 0 2 "22u" 1 "" 0 "neutral" 0>
  <C C1 1 260 270 -26 -49 0 2 "22u" 1 "" 0 "neutral" 0>
  <_BJT Q2N6715_1 1 500 350 8 -26 0 0 "npn" 0 "2.22e-13" 0 "0.9956" 0 "0.995" 0 "2" 0 "1" 0 "100" 0 "30" 0 "2.9e-14" 0 "1.35" 0 "2.97e-13" 0 "1.321" 0 "230" 0 "56" 0 "0" 0 "0" 0 "0.069" 0 "0.075" 0 "0.04" 0 "3.18e-10" 0 "0.75" 0 "0.33" 0 "5.1e-11" 0 "0.595" 0 "0.42" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "7.7e-10" 0 "0" 0 "0" 0 "0" 0 "2.7e-08" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "0" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <_BJT Q2N6715_2 1 230 370 -73 -26 1 2 "npn" 0 "2.22e-13" 0 "0.9956" 0 "0.995" 0 "2" 0 "1" 0 "100" 0 "30" 0 "2.9e-14" 0 "1.35" 0 "2.97e-13" 0 "1.321" 0 "230" 0 "56" 0 "0" 0 "0" 0 "0.069" 0 "0.075" 0 "0.04" 0 "3.18e-10" 0 "0.75" 0 "0.33" 0 "5.1e-11" 0 "0.595" 0 "0.42" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "7.7e-10" 0 "0" 0 "0" 0 "0" 0 "2.7e-08" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "0" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <.TR TR1 1 740 140 0 78 0 0 "lin" 1 "0" 1 "5" 1 "5000" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <620 190 620 240 "" 0 0 0 "">
  <500 130 620 130 "" 0 0 0 "">
  <500 130 500 150 "" 0 0 0 "">
  <230 400 230 430 "" 0 0 0 "">
  <500 210 500 270 "Vsq2" 530 200 25 "">
  <200 130 290 130 "" 0 0 0 "">
  <200 190 200 270 "Vsq1" 148 200 35 "">
  <200 270 230 270 "" 0 0 0 "">
  <290 190 290 270 "" 0 0 0 "">
  <460 270 500 270 "" 0 0 0 "">
  <400 130 500 130 "" 0 0 0 "">
  <400 130 400 140 "" 0 0 0 "">
  <400 200 400 270 "" 0 0 0 "">
  <200 270 200 340 "" 0 0 0 "">
  <200 340 230 340 "" 0 0 0 "">
  <260 370 400 370 "" 0 0 0 "">
  <290 130 400 130 "" 0 0 0 "">
  <400 270 400 370 "" 0 0 0 "">
  <500 380 500 430 "" 0 0 0 "">
  <500 270 500 320 "" 0 0 0 "">
  <290 270 290 350 "" 0 0 0 "">
  <290 350 470 350 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect -87 48 1402 338 3 #c0c0c0 1 00 1 0 0.2 5 1 0.870159 5e-08 0.87016 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vsq1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vsq2)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -80 510 9 #000000 0 "https://rayshobby.net/wordpress/learning-electronics-1-multivibrator/\n\nThis is a very simple oscillator circuit that can be used to generate square waves.\nIt consists of only two NPN transistors (T1 and T2), two capacitors (C1 and C2), and four resistors (R1-R4), organized in a symmetric fashion. The collectors of T1 and T2 are outputs, which provide complement square wave signals.\nThe values of the capacitors and resistors can vary depend on the desired frequency, and their values do not have to be symmetric.\n\nThe resistor values should satisfy: R2 / R1 < h_fe, where h_fe is the transistor's current gain (same for R3 and R4). As h_fe is usually around 80 to 250 for 2N3904, the specific values above (R1=1K, R2=100K) are close to the margin.\nTo improve this, consider increasing R1 or decreasing R2. Alternatively, you can replace 2N3904 with BC547, or even a Darlington transistor (e.g. MPSA14), which have higher h_fe values.\n\nUse a real transistor model here, the ideal NPN one defaults has a configuration where we would not get a square wave.">
</Paintings>
