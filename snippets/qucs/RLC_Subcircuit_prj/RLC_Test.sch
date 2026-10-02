<Qucs Schematic 24.2.1>
<Properties>
  <View=-422,-561,1503,348,0.988775,105,0>
  <Grid=10,10,1>
  <DataSet=RLC_Test.dat>
  <DataDisplay=RLC_Test.dpl>
  <OpenDisplay=0>
  <Script=RLC_Test.m>
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
  <Vac V1 1 270 -110 18 -26 0 1 "1 V" 1 "1 kHz" 0 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 270 -10 0 0 0 0>
  <.AC AC1 1 320 80 0 45 0 0 "log" 1 "100 Hz" 1 "100 kHz" 1 "100" 1 "no" 0>
  <Eqn Eqn1 1 140 90 -32 18 0 0 "gain=dB(Vout.v)" 1 "phase=phase(Vout.v)" 1 "yes" 0>
  <Sub RLC1 1 460 -140 -22 34 0 0 "RLC.sch" 0 "1 mH" 1 "0.88uF" 1 "1 Ohm" 1>
</Components>
<Wires>
  <270 -80 270 -10 "" 0 0 0 "">
  <270 -140 390 -140 "Vin" 350 -170 43 "">
  <530 -140 530 -140 "Vout" 550 -170 0 "">
</Wires>
<Diagrams>
  <Rect -120 -250 519 160 3 #c0c0c0 1 10 1 100 1 100000 1 -0.0525257 0.2 0.6 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.v(vout)@frequency" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect -110 -20 240 160 3 #c0c0c0 1 10 1 0 20000 100000 1 -70.9517 50 19.2848 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.gain@frequency" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect -200 210 240 160 3 #c0c0c0 1 10 1 0 20000 100000 1 -2 2 2 1 -1 1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/ac.v(phase)@frequency" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
