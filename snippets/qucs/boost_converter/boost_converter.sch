<Qucs Schematic 24.2.1>
<Properties>
  <View=-1557,-721,2221,648,0.693434,686,63>
  <Grid=10,10,1>
  <DataSet=boost_converter.dat>
  <DataDisplay=boost_converter.dpl>
  <OpenDisplay=0>
  <Script=boost_converter.m>
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
  <Vdc V1 1 170 70 -26 18 0 0 "5V" 1>
  <GND * 1 120 70 0 0 0 3>
  <L L1 1 260 70 -26 10 0 0 "1" 1 "" 0>
  <C C1 1 490 130 17 -26 0 1 "10u" 1 "" 0 "neutral" 0>
  <GND * 1 320 280 0 0 0 0>
  <Diode D_1N5408_1 1 370 70 -26 -61 0 2 "63n" 1 "1.7" 1 "53p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "14.1m" 0 "4.32u" 0 "0" 0 "0" 0 "1" 0 "1" 0 "1k" 0 "10u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <_MOSFET T_BSS123_1 1 320 220 8 -26 0 0 "nfet" 0 "1" 1 "6.37m" 0 "1.24" 0 "0.75" 0 "625u" 0 "0.14" 0 "0.14" 0 "0" 0 "85f" 0 "1" 0 "0.0001" 0 "0.0001" 0 "0" 0 "1e-07" 0 "36n" 0 "30n" 0 "124n" 0 "19.8p" 0 "23.7p" 0 "0.8" 0 "0.46" 0 "0.5" 0 "0" 0 "0.33" 0 "0" 0 "0" 0 "0" 0 "1" 0 "600" 0 "0" 0 "1" 0 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "1" 0 "1" 0 "26.85" 0 "26.85" 0>
  <Vrect V2 1 230 250 -84 -41 0 1 "10 V" 1 "0.01ms" 1 "0.01ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0" 1>
  <R R1 1 630 120 15 -26 0 1 "20K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <.TR TR1 1 760 30 0 91 0 0 "lin" 1 "0" 1 "500ms" 1 "75000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <120 70 140 70 "" 0 0 0 "">
  <200 70 230 70 "" 0 0 0 "">
  <400 70 490 70 "" 0 0 0 "">
  <490 70 490 100 "" 0 0 0 "">
  <490 70 630 70 "" 0 0 0 "">
  <630 70 630 90 "" 0 0 0 "">
  <490 160 490 280 "" 0 0 0 "">
  <290 70 320 70 "" 0 0 0 "">
  <320 70 340 70 "" 0 0 0 "">
  <320 70 320 190 "" 0 0 0 "">
  <230 220 290 220 "" 0 0 0 "">
  <320 280 490 280 "" 0 0 0 "">
  <320 250 320 280 "" 0 0 0 "">
  <230 280 320 280 "" 0 0 0 "">
  <490 280 630 280 "" 0 0 0 "">
  <630 150 630 280 "" 0 0 0 "">
  <630 70 630 70 "Vout" 660 40 0 "">
</Wires>
<Diagrams>
  <Rect -310 -75 1663 645 3 #c0c0c0 1 00 1 0 0.05 0.5 1 3.39295 2 12 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 50 380 9 #000000 0 "A boost converter circuit raises the voltage at the output, it tries it's best to keep the output voltage constant but practically there will be noise in the waveform output.\nThe square wave frequency determines how much the output DC voltage fluctuates, the diode/transistor/resistor determines how much boost the voltage will gain.">
</Paintings>
