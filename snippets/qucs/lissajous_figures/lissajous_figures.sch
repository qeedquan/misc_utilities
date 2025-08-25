<Qucs Schematic 24.2.1>
<Properties>
  <View=-2306,-147,2631,941,0.826446,1639,0>
  <Grid=10,10,1>
  <DataSet=lissajous_figures.dat>
  <DataDisplay=lissajous_figures.dpl>
  <OpenDisplay=0>
  <Script=lissajous_figures.m>
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
  <.TR TR1 1 890 110 0 84 0 0 "lin" 1 "0" 1 "100ms" 1 "10000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Vac V1 1 460 40 18 -26 0 1 "5V" 1 "40" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 460 90 0 0 0 0>
  <Vac V3 1 260 40 18 -26 0 1 "5V" 1 "100" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <Vac V4 1 260 190 18 -26 0 1 "5V" 1 "104" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 260 240 0 0 0 0>
  <GND * 1 260 90 0 0 0 0>
  <Vac V5 1 650 60 18 -26 0 1 "5V" 1 "91" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <Vac V6 1 650 210 18 -26 0 1 "5V" 1 "30" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 650 260 0 0 0 0>
  <GND * 1 650 110 0 0 0 0>
  <Vac V2 1 460 200 18 -26 0 1 "5V" 1 "101" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 460 250 0 0 0 0>
</Components>
<Wires>
  <460 70 460 90 "" 0 0 0 "">
  <260 70 260 90 "" 0 0 0 "">
  <260 220 260 240 "" 0 0 0 "">
  <650 90 650 110 "" 0 0 0 "">
  <650 240 650 260 "" 0 0 0 "">
  <460 230 460 250 "" 0 0 0 "">
  <460 10 460 10 "Vx2" 490 -20 0 "">
  <260 10 260 10 "Vx1" 290 -20 0 "">
  <260 160 260 160 "Vy1" 290 130 0 "">
  <650 30 650 30 "Vx3" 680 0 0 "">
  <650 180 650 180 "Vy3" 680 150 0 "">
  <460 170 460 170 "Vy2" 490 140 0 "">
</Wires>
<Diagrams>
  <Rect -60 699 246 359 3 #c0c0c0 1 00 1 -5 2 5 1 -6 2 6 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vy1)@tran.v(vx1)" #26a269 0 3 0 0 0>
  </Rect>
  <Rect 250 665 508 275 3 #c0c0c0 1 00 1 -5 1 5 1 -6 2 6 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vy2)@tran.v(vx2)" #c64600 0 3 0 0 0>
  </Rect>
  <Rect 820 704 578 344 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vy3)@tran.v(vx3)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 140 800 9 #000000 0 "A Lissajous figure can be generated using two AC sources of different frequencies, then we plot their output as (Vout1, Vout2) coordinate values\nThe AC frequencies determine the shape of the wave.">
</Paintings>
