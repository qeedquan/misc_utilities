<Qucs Schematic 24.2.1>
<Properties>
  <View=-431,-329,1026,403,1.21,6,0>
  <Grid=10,10,1>
  <DataSet=cuk_converter.dat>
  <DataDisplay=cuk_converter.dpl>
  <OpenDisplay=0>
  <Script=cuk_converter.m>
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
  <Vdc V1 1 -280 -140 18 -26 0 1 "5V" 1>
  <L L1 1 -150 -170 -26 10 0 0 "1" 1 "" 0>
  <GND * 1 -280 -80 0 0 0 0>
  <L L2 1 220 -170 -26 10 0 0 "1" 1 "" 0>
  <GND * 1 140 -10 0 0 0 0>
  <C C1 1 30 -170 -26 17 0 0 "10u" 1 "" 0 "neutral" 0>
  <GND * 1 -150 40 0 0 0 0>
  <_MOSFET M2N3797_1 1 -60 -60 8 -26 0 0 "nfet" 0 "3" 0 "1.8m" 0 "3.72" 0 "0.75" 0 "7.5m" 0 "21" 0 "21" 0 "0" 0 "10f" 0 "1" 0 "0.0001" 0 "0.0001" 0 "0" 0 "1e-07" 0 "6n" 0 "5n" 0 "39n" 0 "13.2p" 0 "15.9p" 0 "0.8" 0 "0.46" 0 "0.5" 0 "0" 0 "0.33" 0 "0" 0 "0" 0 "0" 0 "1" 0 "600" 0 "0" 0 "1" 0 "1" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "0" 0 "1" 0 "1" 0 "26.85" 0 "26.85" 0>
  <C C2 1 300 -90 17 -26 0 1 "10u" 1 "" 0 "neutral" 0>
  <R R1 1 420 -90 -43 -26 0 3 "1K" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vrect V2 1 -150 -30 18 -26 0 1 "10 V" 1 "1 ms" 1 "1 ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0V" 1>
  <Diode D_1N5408_1 1 140 -90 13 -26 0 1 "63n" 1 "1.7" 1 "53p" 0 "0.333" 0 "0.7" 0 "0.5" 0 "0" 0 "0" 0 "2" 0 "14.1m" 0 "4.32u" 0 "0" 0 "0" 0 "1" 0 "1" 0 "1k" 0 "10u" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <.TR TR1 1 590 30 0 75 0 0 "lin" 1 "0" 1 "1" 1 "500000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <-280 -110 -280 -80 "" 0 0 0 "">
  <-280 -170 -180 -170 "" 0 0 0 "">
  <250 -170 300 -170 "" 0 0 0 "">
  <140 -170 190 -170 "" 0 0 0 "">
  <60 -170 140 -170 "" 0 0 0 "">
  <-120 -170 -60 -170 "" 0 0 0 "">
  <-60 -170 0 -170 "" 0 0 0 "">
  <-60 -170 -60 -90 "" 0 0 0 "">
  <-60 -30 -60 -10 "" 0 0 0 "">
  <-60 -10 140 -10 "" 0 0 0 "">
  <140 -10 300 -10 "" 0 0 0 "">
  <-150 -60 -90 -60 "" 0 0 0 "">
  <-150 0 -150 40 "" 0 0 0 "">
  <300 -170 300 -120 "" 0 0 0 "">
  <300 -60 300 -10 "" 0 0 0 "">
  <140 -170 140 -120 "" 0 0 0 "">
  <140 -60 140 -10 "" 0 0 0 "">
  <300 -10 420 -10 "" 0 0 0 "">
  <420 -60 420 -10 "" 0 0 0 "">
  <300 -170 420 -170 "" 0 0 0 "">
  <420 -170 420 -120 "" 0 0 0 "">
  <420 -170 420 -170 "Vout" 450 -200 0 "">
</Wires>
<Diagrams>
  <Rect 550 -47 264 233 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout)@time" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -270 150 9 #000000 0 "A type of buck-boost converter with low ripple current.\nA Cuk converter can be seen as a combination of boost converter and buck converter,\nhaving one switching device and a mutual capacitor, to couple the energy.\n\nSimilar to the buck-boost converter with inverting topology,\nthe output voltage of non-isolated Cuk converter is typically inverted, with lower or higher values with respect to the input voltage. ">
</Paintings>
