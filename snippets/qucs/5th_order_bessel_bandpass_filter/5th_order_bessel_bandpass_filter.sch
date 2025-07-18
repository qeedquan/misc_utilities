<Qucs Schematic 24.2.1>
<Properties>
  <View=-798,-1091,2478,729,0.564474,0,0>
  <Grid=10,10,1>
  <DataSet=5th_order_bessel_bandpass_filter.dat>
  <DataDisplay=5th_order_bessel_bandpass_filter.dpl>
  <OpenDisplay=0>
  <Script=5th_order_bessel_bandpass_filter.m>
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
  <GND * 1 380 130 0 0 0 0>
  <L L1 1 380 60 10 -26 0 1 "22.83uH" 1 "" 0>
  <L L2 1 540 -70 -27 -62 0 0 "4.036uH" 1 "" 0>
  <GND * 1 660 130 0 0 0 0>
  <L L3 1 660 60 10 -26 0 1 "4.949uH" 1 "" 0>
  <L L4 1 820 -70 -27 -62 0 0 "8.841uH" 1 "" 0>
  <GND * 1 910 130 0 0 0 0>
  <L L5 1 910 60 10 -26 0 1 "1.762uH" 1 "" 0>
  <GND * 1 1050 120 0 0 0 0>
  <Pac P2 1 1050 50 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 MHz" 0 "26.85" 0 "true" 0>
  <C C1 1 330 60 -98 -33 0 3 "554.9 pF" 1 "" 0 "neutral" 0>
  <Pac P1 1 100 60 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 MHz" 0 "26.85" 0 "true" 0>
  <GND * 1 100 140 0 0 0 0>
  <C C3 1 610 60 -98 -26 0 3 "2.559nF" 1 "" 0 "neutral" 0>
  <C C5 1 860 60 -96 -30 0 3 "7.188nF" 1 "" 0 "neutral" 0>
  <C C4 1 720 -70 -26 -61 0 2 "1.432nF" 1 "" 0 "neutral" 0>
  <C C2 1 440 -70 -26 -61 0 2 "3.138nF" 1 "" 0 "neutral" 0>
  <.SP SP1 1 180 300 0 79 0 0 "log" 1 "100 kHz" 1 "20 MHz" 1 "200" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn1 1 550 330 -32 18 0 0 "dBS21=dB(S[2,1])" 1 "dBS11=dB(S[1,1])" 1 "yes" 0>
</Components>
<Wires>
  <380 90 380 130 "" 0 0 0 "">
  <330 30 380 30 "" 0 0 0 "">
  <330 90 380 90 "" 0 0 0 "">
  <470 -70 510 -70 "" 0 0 0 "">
  <380 -70 410 -70 "" 0 0 0 "">
  <380 -70 380 30 "" 0 0 0 "">
  <660 90 660 130 "" 0 0 0 "">
  <610 30 660 30 "" 0 0 0 "">
  <610 90 660 90 "" 0 0 0 "">
  <850 -70 910 -70 "" 0 0 0 "">
  <750 -70 790 -70 "" 0 0 0 "">
  <660 -70 690 -70 "" 0 0 0 "">
  <660 -70 660 30 "" 0 0 0 "">
  <570 -70 660 -70 "" 0 0 0 "">
  <910 90 910 130 "" 0 0 0 "">
  <860 30 910 30 "" 0 0 0 "">
  <860 90 910 90 "" 0 0 0 "">
  <910 -70 910 30 "" 0 0 0 "">
  <910 -70 1050 -70 "" 0 0 0 "">
  <1050 80 1050 120 "" 0 0 0 "">
  <1050 -70 1050 20 "" 0 0 0 "">
  <100 -70 380 -70 "" 0 0 0 "">
  <100 90 100 140 "" 0 0 0 "">
  <100 -70 100 30 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Smith -150 -534 296 296 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"S[1,1]" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 260 -511 339 339 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"S[1,2]" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 670 -501 339 339 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"S[2,1]" #0000ff 0 3 0 0 0>
  </Smith>
  <Smith 1070 -491 339 339 3 #c0c0c0 1 00 1 0 1 1 1 0 4 1 1 0 1 1 315 0 225 1 0 0 "" "" "">
	<"S[2,2]" #0000ff 0 3 0 0 0>
  </Smith>
  <Rect -220 -216 567 214 3 #c0c0c0 1 10 1 0 5e+06 2e+07 1 -119.81 50 10.8875 1 -1 1 1 315 0 225 1 0 0 "frequency" "" "">
	<"dBS21@frequency" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 430 -400 12 #000000 0 "Bessel filters are implemented using LC ladders, the order of the filter is how many LC pairs components used.\nUsually use a design tool to get the values for the L and C components.\n\nThis specific 5th order bessel filter act like a bandpass filter for frequencies 1-2 Mhz\nhttps://markimicrowave.com/technical-resources/tools/lc-filter-design-tool/\n">
</Paintings>
