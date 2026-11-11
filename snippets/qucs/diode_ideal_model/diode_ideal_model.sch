<Qucs Schematic 24.2.1>
<Properties>
  <View=-420,-715,1895,1047,0.679516,0,298>
  <Grid=10,10,1>
  <DataSet=diode_ideal_model.dat>
  <DataDisplay=diode_ideal_model.dpl>
  <OpenDisplay=0>
  <Script=diode.m>
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
  <Vdc V1 1 260 240 18 -26 0 1 "1 V" 1>
  <GND * 1 260 310 0 0 0 0>
  <IProbe Pr1 1 370 140 -26 16 0 0>
  <GND * 1 790 230 0 0 0 0>
  <.DC DC1 1 170 400 0 47 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Diode D1 1 500 140 -26 -103 0 2 "1e-15 A" 1 "1" 1 "10 fF" 1 "0.5" 0 "0.7 V" 0 "0.5" 0 "0.0 fF" 0 "0.0" 0 "2.0" 0 "0.0 Ohm" 0 "0.0 ps" 0 "0" 0 "0.0" 0 "1.0" 0 "1.0" 0 "10" 0 "1 mA" 0 "26.85" 0 "3.0" 0 "1.11" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "0.0" 0 "26.85" 0 "1.0" 0 "normal" 0>
  <.SW SW1 1 180 500 0 79 0 0 "DC1" 1 "lin" 1 "V1" 1 "0" 1 "4" 1 "10000" 1 "false" 0>
</Components>
<Wires>
  <260 270 260 310 "" 0 0 0 "">
  <260 140 260 210 "" 0 0 0 "">
  <260 140 340 140 "" 0 0 0 "">
  <400 140 470 140 "" 0 0 0 "">
  <790 140 790 230 "" 0 0 0 "">
  <530 140 790 140 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 510 752 715 332 3 #c0c0c0 1 00 0 0 0.5 2 0 0 100 1000 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/sw1.i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 130 -660 12 #000000 0 "Shows the nonlinear behavior of the diode for current vs voltage.\nThere is a voltage threshold, denoted as the forward voltage where the current suddenly ramps up in an exponential fashion\n\nDiode modeling parameters:\n\nI_S - The saturation current, represents the reverse-biased current that flows due to minority carriers in the semiconductor.\nIt's a tiny current, often in the nano or picoampere range, and is present even in the absence of external voltage because of thermal energy.\n\nN - Emission Coefficient,the ideality factor, determines how closely the diode follows the ideal diode equation.\nA value of (N = 1) suggests ideal diode behavior, while values > 1 suggest some non-idealities due to recombination, generation, or tunneling processes.\n\nBV - Breakdown voltage is the reverse voltage at which the diode starts conducting a significant reverse current.\nFor Zener diodes, this is the voltage at which the Zener effect (or avalanche breakdown for higher voltage Zeners) takes place.\n\nUsually you would want to be able to specify the forward voltage for the diode, but that value is represented by the combination of the other diode parameters.\nYou have to specify the forward voltage in an indirect manner by setting the other parameters to be the right values.\nYou can use things like the diode equation to find the parameters.\nThe saturation current is one of the dominant parameter controlling the forward voltage\n\nThe simplified equation people learn in school:\nI = I0*(exp((q*V)/(k*T)) - 1)\nI = the net current flowing through the diode\nI0 = "dark saturation current", the diode leakage current density in the absence of light\nV = applied voltage across the terminals of the diode\nq = absolute value of electron charge\nk = Boltzmann's constant\nT = absolute temperature (K).">
</Paintings>
