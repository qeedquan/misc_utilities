<Qucs Schematic 24.2.1>
<Properties>
  <View=-2245,-776,2463,662,0.826446,1791,240>
  <Grid=10,10,1>
  <DataSet=rlc_parallel_resonance.dat>
  <DataDisplay=rlc_parallel_resonance.dpl>
  <OpenDisplay=0>
  <Script=rlc_parallel_resonance.m>
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
  <GND * 1 310 170 0 0 0 0>
  <R R1 1 420 130 -26 15 0 0 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 510 70 15 -26 0 1 "2K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <L L1 1 620 70 -37 -26 0 3 "1H" 1 "" 0>
  <C C1 1 730 70 17 -26 0 1 "15u" 1 "" 0 "neutral" 0>
  <Vac V1 1 310 70 18 -26 0 1 "5 V" 1 "30" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 310 400 0 0 0 0>
  <R R3 1 420 360 -26 15 0 0 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R4 1 510 300 15 -26 0 1 "2K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <L L2 1 620 300 -37 -26 0 3 "1H" 1 "" 0>
  <C C2 1 730 300 17 -26 0 1 "15u" 1 "" 0 "neutral" 0>
  <GND * 1 300 620 0 0 0 0>
  <R R5 1 410 580 -26 15 0 0 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R6 1 500 520 15 -26 0 1 "2K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <L L3 1 610 520 -37 -26 0 3 "1H" 1 "" 0>
  <C C3 1 720 520 17 -26 0 1 "15u" 1 "" 0 "neutral" 0>
  <Vac V2 1 310 300 18 -26 0 1 "5 V" 1 "41.1" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <Vac V3 1 300 520 18 -26 0 1 "5 V" 1 "50" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <IProbe Pr1 1 680 10 -26 16 0 0>
  <IProbe Pr2 1 680 240 -26 16 0 0>
  <IProbe Pr3 1 670 460 -26 16 0 0>
  <.TR TR1 1 960 140 0 84 0 0 "lin" 1 "0" 1 "100ms" 1 "10000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <510 10 620 10 "" 0 0 0 "">
  <450 130 510 130 "" 0 0 0 "">
  <310 130 390 130 "" 0 0 0 "">
  <310 130 310 170 "" 0 0 0 "">
  <510 130 620 130 "" 0 0 0 "">
  <510 10 510 40 "" 0 0 0 "">
  <510 100 510 130 "" 0 0 0 "">
  <620 100 620 130 "" 0 0 0 "">
  <620 10 620 40 "" 0 0 0 "">
  <620 130 730 130 "" 0 0 0 "">
  <730 100 730 130 "" 0 0 0 "">
  <310 100 310 130 "" 0 0 0 "">
  <310 10 510 10 "" 0 0 0 "">
  <310 10 310 40 "" 0 0 0 "">
  <510 240 620 240 "" 0 0 0 "">
  <310 360 390 360 "" 0 0 0 "">
  <310 360 310 400 "" 0 0 0 "">
  <450 360 510 360 "" 0 0 0 "">
  <510 240 510 270 "" 0 0 0 "">
  <510 360 620 360 "" 0 0 0 "">
  <510 330 510 360 "" 0 0 0 "">
  <620 330 620 360 "" 0 0 0 "">
  <620 240 620 270 "" 0 0 0 "">
  <620 240 650 240 "" 0 0 0 "">
  <730 240 730 270 "" 0 0 0 "">
  <620 360 730 360 "" 0 0 0 "">
  <730 330 730 360 "" 0 0 0 "">
  <310 330 310 360 "" 0 0 0 "">
  <310 240 510 240 "" 0 0 0 "">
  <310 240 310 270 "" 0 0 0 "">
  <500 460 610 460 "" 0 0 0 "">
  <300 580 380 580 "" 0 0 0 "">
  <300 580 300 620 "" 0 0 0 "">
  <500 460 500 490 "" 0 0 0 "">
  <440 580 500 580 "" 0 0 0 "">
  <500 580 610 580 "" 0 0 0 "">
  <500 550 500 580 "" 0 0 0 "">
  <610 550 610 580 "" 0 0 0 "">
  <610 460 610 490 "" 0 0 0 "">
  <610 580 720 580 "" 0 0 0 "">
  <720 550 720 580 "" 0 0 0 "">
  <300 550 300 580 "" 0 0 0 "">
  <300 460 500 460 "" 0 0 0 "">
  <300 460 300 490 "" 0 0 0 "">
  <730 10 730 40 "" 0 0 0 "">
  <710 10 730 10 "" 0 0 0 "">
  <620 10 650 10 "" 0 0 0 "">
  <710 240 730 240 "" 0 0 0 "">
  <720 460 720 490 "" 0 0 0 "">
  <700 460 720 460 "" 0 0 0 "">
  <610 460 640 460 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 20 -110 1648 660 3 #c0c0c0 1 00 1 0 0.1 1 1 -0.0157933 0.01 0.0159307 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.i(pr1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.i(pr2)@time" #ff0000 0 3 0 0 0>
	<"ngspice/tran.i(pr3)@time" #ff00ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 850 30 9 #000000 0 "In a parallel RLC circuit, resonance occurs at the maximum impedance, meaning the current will be the smallest value at resonance frequency">
</Paintings>
