EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Non Inverting OpAmp"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:R R1
U 1 1 612A5215
P 5950 4000
F 0 "R1" H 6020 4046 50  0000 L CNN
F 1 "R" H 6020 3955 50  0000 L CNN
F 2 "" V 5880 4000 50  0001 C CNN
F 3 "~" H 5950 4000 50  0001 C CNN
	1    5950 4000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 612A577B
P 6450 2950
F 0 "R2" H 6520 2996 50  0000 L CNN
F 1 "R" H 6520 2905 50  0000 L CNN
F 2 "" V 6380 2950 50  0001 C CNN
F 3 "~" H 6450 2950 50  0001 C CNN
	1    6450 2950
	0    -1   -1   0   
$EndComp
$Comp
L pspice:OPAMP U1
U 1 1 612A62EF
P 6550 3550
F 0 "U1" H 6894 3596 50  0000 L CNN
F 1 "OPAMP" H 6894 3505 50  0000 L CNN
F 2 "" H 6550 3550 50  0001 C CNN
F 3 "~" H 6550 3550 50  0001 C CNN
	1    6550 3550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 612A692A
P 5950 4300
F 0 "#PWR01" H 5950 4050 50  0001 C CNN
F 1 "GND" H 5955 4127 50  0000 C CNN
F 2 "" H 5950 4300 50  0001 C CNN
F 3 "" H 5950 4300 50  0001 C CNN
	1    5950 4300
	1    0    0    -1  
$EndComp
Text GLabel 5350 3450 0    50   Input ~ 0
Vin
Text GLabel 7600 3550 2    50   Input ~ 0
Vout
Wire Wire Line
	5950 4300 5950 4150
Wire Wire Line
	5950 3850 5950 3650
Wire Wire Line
	5950 3650 6250 3650
Wire Wire Line
	5950 3650 5950 2950
Wire Wire Line
	5950 2950 6300 2950
Connection ~ 5950 3650
Wire Wire Line
	6850 3550 7300 3550
Wire Wire Line
	6600 2950 7300 2950
Wire Wire Line
	7300 2950 7300 3550
Connection ~ 7300 3550
Wire Wire Line
	7300 3550 7600 3550
Wire Wire Line
	5350 3450 6250 3450
Text Notes 5250 5050 0    50   ~ 0
For non-inverting opamp, the gain equation is\nGain = clamp(1 + R2/R1, saturation_value)\nThe output is a scaled version of the input based on the resistor.\nIt does not flip the output (in phase) and does not change the frequency).\nVin can be a AC/DC voltage source.\nVout is Vin*Gain, where the gain can be clipped to the saturation voltage if the input is too large.\n
NoConn ~ 6450 3850
NoConn ~ 6450 3250
$EndSCHEMATC
