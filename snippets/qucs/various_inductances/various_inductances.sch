<Qucs Schematic 24.2.1>
<Properties>
  <View=-1936,89,3850,2163,0.573086,844,0>
  <Grid=10,10,1>
  <DataSet=various_inductances.dat>
  <DataDisplay=various_inductances.dpl>
  <OpenDisplay=0>
  <Script=various_inductances.m>
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
  <Vac V1 1 240 250 18 -26 0 1 "5 V" 1 "80 Hz" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 370 320 0 0 0 0>
  <L L1 1 480 270 19 -20 0 3 "1 H" 1 "" 0>
  <GND * 1 380 640 0 0 0 0>
  <Vac V2 1 240 410 18 -26 0 1 "5 V" 1 "80 Hz" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 370 480 0 0 0 0>
  <Vac V3 1 240 570 18 -26 0 1 "5 V" 1 "80 Hz" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <L L3 1 480 590 19 -20 0 3 "20 mH" 1 "" 0>
  <L L2 1 480 430 19 -20 0 3 "400 mH" 1 "" 0>
  <R R1 1 360 220 -26 15 0 0 "100 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 360 380 -26 15 0 0 "100 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 360 540 -26 15 0 0 "100 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.TR TR1 1 620 300 0 83 0 0 "lin" 1 "0" 1 "80" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <370 320 480 320 "" 0 0 0 "">
  <240 320 370 320 "" 0 0 0 "">
  <240 280 240 320 "" 0 0 0 "">
  <480 220 480 240 "" 0 0 0 "">
  <390 220 480 220 "" 0 0 0 "">
  <240 220 330 220 "" 0 0 0 "">
  <480 300 480 320 "" 0 0 0 "">
  <370 480 480 480 "" 0 0 0 "">
  <240 480 370 480 "" 0 0 0 "">
  <240 440 240 480 "" 0 0 0 "">
  <480 380 480 400 "" 0 0 0 "">
  <390 380 480 380 "" 0 0 0 "">
  <240 380 330 380 "" 0 0 0 "">
  <480 460 480 480 "" 0 0 0 "">
  <380 640 480 640 "" 0 0 0 "">
  <480 620 480 640 "" 0 0 0 "">
  <240 640 380 640 "" 0 0 0 "">
  <240 600 240 640 "" 0 0 0 "">
  <480 540 480 560 "" 0 0 0 "">
  <390 540 480 540 "" 0 0 0 "">
  <240 540 330 540 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect -230 1552 1996 862 3 #c0c0c0 1 00 1 0 5 80 1 -0.06 0.02 0.06 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"V1.It" #0000ff 0 3 0 0 0>
	<"V2.It" #ff0000 0 3 0 0 0>
	<"V3.It" #ff00ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 600 530 12 #000000 0 "Changing inductances size in a series RL circuit changes the magnitude/phase of the signal.\nActs like a filter.">
</Paintings>
