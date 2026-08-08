<Qucs Schematic 24.2.1>
<Properties>
  <View=-236,-229,2032,1193,1.01526,412,0>
  <Grid=10,10,1>
  <DataSet=colpitts_oscillator.dat>
  <DataDisplay=colpitts_oscillator.dpl>
  <OpenDisplay=0>
  <Script=colpitts_oscillator.m>
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
  <Vdc V1 1 490 0 -26 18 0 0 "5" 1>
  <GND * 1 450 0 0 0 0 3>
  <_BJT T_2N2222_1 1 550 180 12 -32 0 0 "npn" 0 "1e-14" 0 "1" 0 "1" 0 "0.3" 0 "0" 0 "100" 0 "0" 0 "0" 0 "1.5" 0 "0" 0 "2" 0 "200" 0 "3" 0 "0" 0 "0" 0 "3" 0 "1" 0 "10" 0 "25e-12" 0 "0.75" 0 "0.33" 0 "8e-12" 0 "0.75" 0 "0.33" 0 "1.0" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "400e-12" 0 "3" 0 "0.0" 0 "2" 0 "100e-9" 0 "26.85" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "0.0" 0 "0.0" 0 "3.0" 0 "1.11" 0 "26.85" 0 "1.0" 0>
  <C C1 1 390 210 17 -26 0 1 "100u" 1 "" 0 "neutral" 0>
  <C C2 1 390 300 17 -26 0 1 "100u" 1 "" 0 "neutral" 0>
  <GND * 1 550 310 0 0 0 0>
  <L L1 1 280 250 -47 -26 0 3 "2" 1 "" 0>
  <R R1 1 550 50 15 -26 0 1 "100" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.TR TR1 1 950 140 0 79 0 0 "lin" 1 "0" 1 "2" 1 "5000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R R2 1 670 240 15 -26 0 1 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.FFT FFT1 1 1170 140 0 78 0 0 "100" 1 "1" 1 "hamming" 1 "2" 0 "0.5" 0>
</Components>
<Wires>
  <450 0 460 0 "" 0 0 0 "">
  <520 0 550 0 "" 0 0 0 "">
  <550 0 550 20 "" 0 0 0 "">
  <550 80 550 110 "" 0 0 0 "">
  <550 110 550 150 "" 0 0 0 "">
  <550 110 670 110 "" 0 0 0 "">
  <670 110 670 210 "" 0 0 0 "">
  <390 180 520 180 "" 0 0 0 "">
  <390 240 390 260 "" 0 0 0 "">
  <550 210 550 260 "" 0 0 0 "">
  <390 260 390 270 "" 0 0 0 "">
  <550 260 550 310 "" 0 0 0 "">
  <390 260 550 260 "" 0 0 0 "">
  <390 330 390 360 "" 0 0 0 "">
  <280 180 280 220 "" 0 0 0 "">
  <280 180 390 180 "" 0 0 0 "">
  <280 360 390 360 "" 0 0 0 "">
  <280 280 280 360 "" 0 0 0 "">
  <390 360 670 360 "" 0 0 0 "">
  <670 270 670 360 "" 0 0 0 "">
  <670 110 670 110 "Vout" 700 80 0 "">
</Wires>
<Diagrams>
  <Rect 190 1122 1302 492 3 #c0c0c0 1 00 1 0 0.1 2 1 -0.204096 0.5 5.09666 1 -1 0.2 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 840 542 601 192 3 #c0c0c0 1 00 1 0 10 102.5 1 -0.216029 1 2.40295 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.v(vout)@frequency" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 610 -180 9 #000000 0 "The Colpitt’s oscillator is designed for generation of high frequency sinusoidal oscillations (radio frequencies ranging from 10KHz to 100MHz).\nThey are widely used in commercial signal generators up to 100MHz. Colpitt's oscillator is same as Hartley oscillator except for one difference.\nInstead of using a tapped inductance, Colpitt's oscillator uses a tapped capacitance (centered)\n\nA center tap (CT) is a contact made to a point halfway along a winding of a transformer or inductor, or along the element of a resistor or a potentiometer.\nA tapped inductor is a coil to which electrical access is available at more points.\n\nThe resistors can affect the amplitude/frequency\nThe approximate frequency is calculated as:\nC=(C1*C2)/(C1+C2)\nf0=1/(2*pi*sqrt(L*C))">
</Paintings>
