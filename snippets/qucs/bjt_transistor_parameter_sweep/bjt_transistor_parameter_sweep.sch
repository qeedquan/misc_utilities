<Qucs Schematic 24.2.1>
<Properties>
  <View=-557,-586,1550,434,1.29168,678,359>
  <Grid=10,10,1>
  <DataSet=bjt_transistor_parameter_sweep.dat>
  <DataDisplay=bjt_transistor_parameter_sweep.dpl>
  <OpenDisplay=0>
  <Script=bjt_transistor_parameter_sweep.m>
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
  <Vdc V1 1 620 250 18 -26 0 1 "10 V" 1>
  <_BJT Q2N4401_1 1 430 70 8 -26 0 0 "npn" 0 "9.09e-15" 0 "1" 0 "1" 0 "0.36" 0 "0.54" 0 "113" 0 "24" 0 "1.06e-11" 0 "2" 0 "0" 0 "2" 0 "300" 0 "4" 0 "0" 0 "0" 0 "0.127" 0 "0.319" 0 "1.27" 0 "2.34e-11" 0 "0.75" 0 "0.33" 0 "1.02e-11" 0 "0.75" 0 "0.33" 0 "1" 0 "0" 0 "0.75" 0 "0" 0 "0.5" 0 "5.12e-10" 0 "0" 0 "0" 0 "0" 0 "1.51e-07" 0 "26.85" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1" 0 "1" 0 "0" 0 "1.5" 0 "3" 0 "1.11" 0 "26.85" 0 "1" 0>
  <Idc I1 1 240 70 -26 18 0 0 "Ib" 1>
  <GND * 1 430 330 0 0 0 0>
  <Eqn Eqn1 1 790 140 -32 18 0 0 "Ic=-V1.I" 1 "Beta=Ic/Ib" 1 "Beta_vs_Ic=PlotVs(Beta, Ic)" 1 "yes" 0>
  <.DC DC1 1 760 320 0 46 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 760 -150 0 78 0 0 "DC1" 1 "log" 1 "Ib" 1 "10 n" 1 "10 m" 1 "101" 1 "false" 0>
</Components>
<Wires>
  <620 280 620 330 "" 0 0 0 "">
  <150 330 430 330 "" 0 0 0 "">
  <150 70 150 330 "" 0 0 0 "">
  <150 70 210 70 "" 0 0 0 "">
  <270 70 400 70 "" 0 0 0 "">
  <430 330 620 330 "" 0 0 0 "">
  <430 100 430 330 "" 0 0 0 "">
  <430 -10 430 40 "" 0 0 0 "">
  <430 -10 620 -10 "" 0 0 0 "">
  <620 -10 620 220 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 120 -87 517 203 3 #c0c0c0 1 10 1 3e-09 1 1 1 -23.6054 100 300 1 -1 0.5 1 315 0 225 1 0 0 "Ic" "" "">
	<"Beta_vs_Ic@Ic" #ed333b 0 3 0 0 0>
	  <Mkr 0.0338954 488 -229 3 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 130 -550 12 #000000 0 "BJT Transistor Parameter Sweep\n\nNeed to use the Qucsator simulator, since other SPICE simulators don't support some of the simulation options\n\nThis simulation finds the current transfer curve of the BJT. The input current (base) is given by the swept parameter Ib.\nThe output current at the collector flows through the DC voltage source (V1).\nBetaDC = f(Ic) = Ic/Ib (Gain of input vs output)\n\nPlot on a log scale to see the curve more properly">
</Paintings>
