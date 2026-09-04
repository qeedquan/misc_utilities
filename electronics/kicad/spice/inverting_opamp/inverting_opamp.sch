EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Inverting Opamp"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L pspice:R R1
U 1 1 5E939AB7
P 4400 3250
F 0 "R1" V 4195 3250 50  0000 C CNN
F 1 "1k" V 4286 3250 50  0000 C CNN
F 2 "" H 4400 3250 50  0001 C CNN
F 3 "~" H 4400 3250 50  0001 C CNN
	1    4400 3250
	0    1    1    0   
$EndComp
$Comp
L pspice:R R2
U 1 1 5E93AC74
P 5350 4150
F 0 "R2" V 5145 4150 50  0000 C CNN
F 1 "100k" V 5236 4150 50  0000 C CNN
F 2 "" H 5350 4150 50  0001 C CNN
F 3 "~" H 5350 4150 50  0001 C CNN
	1    5350 4150
	0    1    1    0   
$EndComp
$Comp
L pspice:0 #GND?
U 1 1 5E93B629
P 6450 4500
F 0 "#GND?" H 6450 4400 50  0001 C CNN
F 1 "0" H 6450 4589 50  0000 C CNN
F 2 "" H 6450 4500 50  0001 C CNN
F 3 "~" H 6450 4500 50  0001 C CNN
	1    6450 4500
	1    0    0    -1  
$EndComp
$Comp
L pspice:0 #GND?
U 1 1 5E93B8FD
P 7500 4500
F 0 "#GND?" H 7500 4400 50  0001 C CNN
F 1 "0" H 7500 4589 50  0000 C CNN
F 2 "" H 7500 4500 50  0001 C CNN
F 3 "~" H 7500 4500 50  0001 C CNN
	1    7500 4500
	1    0    0    -1  
$EndComp
$Comp
L pspice:0 #GND?
U 1 1 5E93BA61
P 8350 4500
F 0 "#GND?" H 8350 4400 50  0001 C CNN
F 1 "0" H 8350 4589 50  0000 C CNN
F 2 "" H 8350 4500 50  0001 C CNN
F 3 "~" H 8350 4500 50  0001 C CNN
	1    8350 4500
	1    0    0    -1  
$EndComp
$Comp
L pspice:0 #GND?
U 1 1 5E93BC0E
P 5000 2700
F 0 "#GND?" H 5000 2600 50  0001 C CNN
F 1 "0" H 5000 2789 50  0000 C CNN
F 2 "" H 5000 2700 50  0001 C CNN
F 3 "~" H 5000 2700 50  0001 C CNN
	1    5000 2700
	-1   0    0    1   
$EndComp
$Comp
L pspice:VSOURCE Vin
U 1 1 5E93C033
P 6450 4000
F 0 "Vin" H 6678 4046 50  0000 L CNN
F 1 "dc 0 ac 1" H 6678 3955 50  0000 L CNN
F 2 "" H 6450 4000 50  0001 C CNN
F 3 "~" H 6450 4000 50  0001 C CNN
	1    6450 4000
	1    0    0    -1  
$EndComp
$Comp
L pspice:VSOURCE Vp
U 1 1 5E93CB22
P 7500 4000
F 0 "Vp" H 7728 4046 50  0000 L CNN
F 1 "5" H 7728 3955 50  0000 L CNN
F 2 "" H 7500 4000 50  0001 C CNN
F 3 "~" H 7500 4000 50  0001 C CNN
	1    7500 4000
	1    0    0    -1  
$EndComp
$Comp
L pspice:VSOURCE Vm
U 1 1 5E93D3DD
P 8350 4000
F 0 "Vm" H 8578 4046 50  0000 L CNN
F 1 "-5" H 8578 3955 50  0000 L CNN
F 2 "" H 8350 4000 50  0001 C CNN
F 3 "~" H 8350 4000 50  0001 C CNN
	1    8350 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8350 4300 8350 4500
Wire Wire Line
	7500 4300 7500 4500
Wire Wire Line
	6450 4300 6450 4500
$Comp
L Amplifier_Operational:LF356 U1
U 1 1 5E94BDD0
P 5400 3300
F 0 "U1" H 5744 3346 50  0000 L CNN
F 1 "LF356" H 5744 3255 50  0000 L CNN
F 2 "" H 5450 3350 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lf357.pdf" H 5550 3450 50  0001 C CNN
F 4 "X" H 5400 3300 50  0001 C CNN "Spice_Primitive"
F 5 "LF356/NS" H 5400 3300 50  0001 C CNN "Spice_Model"
F 6 "Y" H 5400 3300 50  0001 C CNN "Spice_Netlist_Enabled"
F 7 "LF356.MOD" H 5400 3300 50  0001 C CNN "Spice_Lib_File"
F 8 "3 2 7 4 6" H 5400 3300 50  0001 C CNN "Spice_Node_Sequence"
	1    5400 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 3200 5000 3200
Wire Wire Line
	5000 3200 5000 2700
Text GLabel 5300 2750 1    50   Input ~ 0
Vp
Text GLabel 5300 3650 3    50   Input ~ 0
Vm
Text GLabel 3950 3250 0    50   Input ~ 0
in
Text GLabel 6050 3300 2    50   Output ~ 0
out
Wire Wire Line
	3950 3250 4150 3250
Wire Wire Line
	4650 3250 4850 3250
Wire Wire Line
	5100 3250 5100 3400
Wire Wire Line
	4850 4150 5100 4150
Connection ~ 4850 3250
Wire Wire Line
	4850 3250 5100 3250
Wire Wire Line
	5600 4150 5700 4150
Wire Wire Line
	5700 3300 6050 3300
Wire Wire Line
	5300 2750 5300 3000
Wire Wire Line
	5700 3300 5700 4150
Connection ~ 5700 3300
Wire Wire Line
	4850 3250 4850 4150
Wire Wire Line
	5300 3650 5300 3600
Text Notes 4100 4500 0    50   ~ 0
.dc Vin -0.5 0.5 0.1
Text GLabel 7500 3600 1    50   Input ~ 0
Vp
Text GLabel 8350 3600 1    50   Input ~ 0
Vm
Text GLabel 6450 3600 1    50   Input ~ 0
in
Wire Wire Line
	6450 3700 6450 3600
Wire Wire Line
	7500 3700 7500 3600
Wire Wire Line
	8350 3700 8350 3600
$EndSCHEMATC
