EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Inverting OpAmp"
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
U 1 1 612AA36B
P 4900 2550
F 0 "R1" H 4970 2596 50  0000 L CNN
F 1 "R" H 4970 2505 50  0000 L CNN
F 2 "" V 4830 2550 50  0001 C CNN
F 3 "~" H 4900 2550 50  0001 C CNN
	1    4900 2550
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 612AA791
P 5400 3000
F 0 "R2" H 5470 3046 50  0000 L CNN
F 1 "R" H 5470 2955 50  0000 L CNN
F 2 "" V 5330 3000 50  0001 C CNN
F 3 "~" H 5400 3000 50  0001 C CNN
	1    5400 3000
	0    -1   -1   0   
$EndComp
$Comp
L pspice:OPAMP U1
U 1 1 612AA86E
P 6100 2450
F 0 "U1" H 6444 2496 50  0000 L CNN
F 1 "OPAMP" H 6444 2405 50  0000 L CNN
F 2 "" H 6100 2450 50  0001 C CNN
F 3 "~" H 6100 2450 50  0001 C CNN
	1    6100 2450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 612AAE27
P 5500 2000
F 0 "#PWR01" H 5500 1750 50  0001 C CNN
F 1 "GND" H 5505 1827 50  0000 C CNN
F 2 "" H 5500 2000 50  0001 C CNN
F 3 "" H 5500 2000 50  0001 C CNN
	1    5500 2000
	-1   0    0    1   
$EndComp
Text GLabel 4550 2550 0    50   Input ~ 0
Vin
Text GLabel 6850 2450 2    50   Input ~ 0
Vout
Wire Wire Line
	4550 2550 4750 2550
Wire Wire Line
	5050 2550 5250 2550
Wire Wire Line
	5250 2550 5250 3000
Wire Wire Line
	5550 3000 5800 3000
Wire Wire Line
	5800 3000 5800 2550
Wire Wire Line
	6400 2450 6750 2450
Wire Wire Line
	5800 3000 6750 3000
Wire Wire Line
	6750 3000 6750 2450
Connection ~ 5800 3000
Connection ~ 6750 2450
Wire Wire Line
	6750 2450 6850 2450
Wire Wire Line
	5500 2000 5500 2350
Wire Wire Line
	5500 2350 5800 2350
NoConn ~ 6000 2150
NoConn ~ 6000 2750
Text Notes 4350 4000 0    50   ~ 0
Inverting Opamp gain equation is\nGain = -R2/R1\nThe difference between inverting and non-inverting opamp is the phase flip,\nthe inverting opamp scales it by -1 (phase flip of 180 degrees).\nThe topology of the resistor is the same except that for inverting opamp, the\nresistor and Vin is connected to the negative side of the opamp and GND is connected to the positive side of opamp
$EndSCHEMATC
