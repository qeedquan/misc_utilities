EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "AC to DC Power Supply"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:AC #PWR01
U 1 1 6120835B
P 3600 3650
F 0 "#PWR01" H 3600 3550 50  0001 C CNN
F 1 "AC" V 3600 3879 50  0000 L CNN
F 2 "" H 3600 3650 50  0001 C CNN
F 3 "" H 3600 3650 50  0001 C CNN
	1    3600 3650
	0    -1   -1   0   
$EndComp
$Comp
L Device:Fuse F1
U 1 1 612088ED
P 4050 3400
F 0 "F1" V 3853 3400 50  0000 C CNN
F 1 "Fuse" V 3944 3400 50  0000 C CNN
F 2 "" V 3980 3400 50  0001 C CNN
F 3 "~" H 4050 3400 50  0001 C CNN
	1    4050 3400
	0    1    1    0   
$EndComp
$Comp
L pspice:DIODE D1
U 1 1 6120BD5B
P 6000 3400
F 0 "D1" H 6000 3665 50  0000 C CNN
F 1 "DIODE" H 6000 3574 50  0000 C CNN
F 2 "" H 6000 3400 50  0001 C CNN
F 3 "~" H 6000 3400 50  0001 C CNN
	1    6000 3400
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C1
U 1 1 6120CF5D
P 6500 3550
F 0 "C1" H 6615 3596 50  0000 L CNN
F 1 "CP1" H 6615 3505 50  0000 L CNN
F 2 "" H 6500 3550 50  0001 C CNN
F 3 "~" H 6500 3550 50  0001 C CNN
	1    6500 3550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 6120DEDF
P 6950 3550
F 0 "R1" H 7020 3596 50  0000 L CNN
F 1 "R" H 7020 3505 50  0000 L CNN
F 2 "" V 6880 3550 50  0001 C CNN
F 3 "~" H 6950 3550 50  0001 C CNN
	1    6950 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 3650 3600 3400
Wire Wire Line
	3600 3400 3900 3400
Wire Wire Line
	4200 3400 4750 3400
$Comp
L Device:Transformer_1P_1S T1
U 1 1 61209A2F
P 5150 3600
F 0 "T1" H 5150 3981 50  0000 C CNN
F 1 "Transformer_1P_1S" H 5150 3890 50  0000 C CNN
F 2 "" H 5150 3600 50  0001 C CNN
F 3 "~" H 5150 3600 50  0001 C CNN
	1    5150 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 3400 5800 3400
Wire Wire Line
	6200 3400 6500 3400
Wire Wire Line
	6500 3400 6950 3400
Connection ~ 6500 3400
Wire Wire Line
	6950 3700 6500 3700
Wire Wire Line
	6500 3700 5550 3700
Wire Wire Line
	5550 3700 5550 3800
Connection ~ 6500 3700
Wire Wire Line
	4750 3800 3600 3800
Wire Wire Line
	3600 3800 3600 3650
Connection ~ 3600 3650
$Comp
L power:-VDC #PWR03
U 1 1 6121A836
P 7300 3700
F 0 "#PWR03" H 7300 3600 50  0001 C CNN
F 1 "-VDC" H 7300 3975 50  0000 C CNN
F 2 "" H 7300 3700 50  0001 C CNN
F 3 "" H 7300 3700 50  0001 C CNN
	1    7300 3700
	-1   0    0    1   
$EndComp
$Comp
L power:+VDC #PWR02
U 1 1 612220D7
P 7300 3400
F 0 "#PWR02" H 7300 3300 50  0001 C CNN
F 1 "+VDC" H 7300 3675 50  0000 C CNN
F 2 "" H 7300 3400 50  0001 C CNN
F 3 "" H 7300 3400 50  0001 C CNN
	1    7300 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3400 7300 3400
Connection ~ 6950 3400
Wire Wire Line
	6950 3700 7300 3700
Connection ~ 6950 3700
Text Notes 3850 4500 0    50   ~ 0
AC flows through T1 to get a higher or lower voltage based on the transformer.\nThe diode D1 forces movement one way to prevent back current.\nThe capacitor C1 gets rid of pulses acting as a filter.\nThe resistor R1 acts as a bleeding resistor for the capacitor to allow discharge when power is off.
$EndSCHEMATC
