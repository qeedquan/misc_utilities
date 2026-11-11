<Qucs Schematic 24.2.1>
<Properties>
  <View=-1348,-686,2264,1348,0.771513,665,204>
  <Grid=10,10,1>
  <DataSet=additive_voltage_sources.dat>
  <DataDisplay=additive_voltage_sources.dpl>
  <OpenDisplay=0>
  <Script=additive_voltage_sources.m>
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
  <.TR TR1 1 30 -300 0 79 0 0 "lin" 1 "0" 1 "10" 1 "200" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <.FFT FFT1 1 230 -300 0 79 0 0 "100kHz" 1 "10kHz" 1 "hanning" 1 "2" 0 "0" 0>
  <Vac V1 1 130 120 18 -26 0 1 "5V" 1 "60k" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 440 60 0 0 0 0>
  <R R1 1 340 -40 -26 15 0 0 "1 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 130 270 0 0 0 0>
  <Vac V4 1 130 220 18 -26 0 1 "10V" 1 "36.8001k" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <Vrect V3 1 130 -10 18 -26 0 1 "2.5825V" 1 "0.25" 1 "1.01688" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0 "0 V" 1>
</Components>
<Wires>
  <370 -40 440 -40 "" 0 0 0 "">
  <440 -40 440 60 "" 0 0 0 "">
  <130 250 130 270 "" 0 0 0 "">
  <130 150 130 190 "" 0 0 0 "">
  <130 20 130 90 "" 0 0 0 "">
  <130 -40 310 -40 "Vadditive" 260 -90 104 "">
</Wires>
<Diagrams>
  <Rect 650 59 704 359 3 #c0c0c0 1 00 1 0 1 10 1 -6 2 6 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vadditive)@time" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 670 402 615 282 3 #c0c0c0 1 00 1 0 10 130 1 -100500 200000 1.1055e+06 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.v(vadditive)@frequency" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text -270 340 12 #000000 0 "Hypothetically, we can chain sources of various waveforms in series to get a combination of them to make new waves">
</Paintings>
