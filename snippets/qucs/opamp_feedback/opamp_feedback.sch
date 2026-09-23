<Qucs Schematic 24.2.1>
<Properties>
  <View=0,-120,1573,839,1,0,60>
  <Grid=10,10,1>
  <DataSet=opamp_feedback.dat>
  <DataDisplay=opamp_feedback.dpl>
  <OpenDisplay=0>
  <Script=opamp_feedback.m>
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
  <Vac V1 1 280 130 -26 18 0 0 "5V" 1 "1 kHz" 0 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 230 130 0 0 0 3>
  <OpAmp OP1 1 520 110 -26 -74 1 0 "1e6" 1 "15 V" 0>
  <GND * 1 190 350 0 0 0 3>
  <OpAmp OP2 1 480 330 -26 -74 1 0 "1e6" 1 "15 V" 0>
  <Vdc V2 1 240 350 -26 18 0 0 "8V" 1>
  <.TR TR1 1 130 460 0 75 0 0 "lin" 1 "0" 1 "10ms" 1 "20000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
</Components>
<Wires>
  <230 130 250 130 "" 0 0 0 "">
  <310 130 490 130 "Vin1" 390 100 45 "">
  <560 110 630 110 "" 0 0 0 "">
  <630 20 630 110 "" 0 0 0 "">
  <460 20 630 20 "" 0 0 0 "">
  <460 20 460 90 "" 0 0 0 "">
  <460 90 490 90 "" 0 0 0 "">
  <190 350 210 350 "" 0 0 0 "">
  <270 350 450 350 "Vin2" 350 320 45 "">
  <520 330 590 330 "" 0 0 0 "">
  <590 240 590 330 "" 0 0 0 "">
  <420 240 590 240 "" 0 0 0 "">
  <420 240 420 310 "" 0 0 0 "">
  <420 310 450 310 "" 0 0 0 "">
  <630 110 630 110 "Vout1" 660 80 0 "">
  <590 330 590 330 "Vout2" 620 300 0 "">
</Wires>
<Diagrams>
  <Rect 810 462 487 252 3 #c0c0c0 1 00 1 0 0.001 0.01 1 -6.29999 5 10 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vout1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vout2)@time" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 810 152 487 252 3 #c0c0c0 1 00 1 0 0.001 0.01 1 -6 2 6 1 -1 0.5 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(vin1)@time" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(vin2)@time" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 280 480 9 #000000 0 "An opamp feedback circuit tries to match the voltage input at the voltage output">
</Paintings>
