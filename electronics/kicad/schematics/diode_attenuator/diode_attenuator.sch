EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Diode Attenuator"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 3050 2100 0    50   Input ~ 0
in
Text GLabel 7800 2100 2    50   Input ~ 0
out
$Comp
L Device:CP1 C1
U 1 1 6126E2F6
P 3500 2100
F 0 "C1" H 3615 2146 50  0000 L CNN
F 1 "CP1" H 3615 2055 50  0000 L CNN
F 2 "" H 3500 2100 50  0001 C CNN
F 3 "~" H 3500 2100 50  0001 C CNN
	1    3500 2100
	0    1    1    0   
$EndComp
$Comp
L Device:R_US R1
U 1 1 612706E5
P 4200 2100
F 0 "R1" H 4268 2146 50  0000 L CNN
F 1 "R_US" H 4268 2055 50  0000 L CNN
F 2 "" V 4240 2090 50  0001 C CNN
F 3 "~" H 4200 2100 50  0001 C CNN
	1    4200 2100
	0    1    1    0   
$EndComp
$Comp
L Device:CP1 C2
U 1 1 612716CA
P 7300 2100
F 0 "C2" H 7415 2146 50  0000 L CNN
F 1 "CP1" H 7415 2055 50  0000 L CNN
F 2 "" H 7300 2100 50  0001 C CNN
F 3 "~" H 7300 2100 50  0001 C CNN
	1    7300 2100
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_US R3
U 1 1 612716D0
P 4800 2250
F 0 "R3" H 4868 2296 50  0000 L CNN
F 1 "R_US" H 4868 2205 50  0000 L CNN
F 2 "" V 4840 2240 50  0001 C CNN
F 3 "~" H 4800 2250 50  0001 C CNN
	1    4800 2250
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery BT1
U 1 1 61271D70
P 4800 2900
F 0 "BT1" H 4908 2946 50  0000 L CNN
F 1 "Battery" H 4908 2855 50  0000 L CNN
F 2 "" V 4800 2960 50  0001 C CNN
F 3 "~" V 4800 2960 50  0001 C CNN
	1    4800 2900
	1    0    0    -1  
$EndComp
$Comp
L Device:D D1
U 1 1 61272AA3
P 5600 2900
F 0 "D1" H 5600 3117 50  0000 C CNN
F 1 "D" H 5600 3026 50  0000 C CNN
F 2 "" H 5600 2900 50  0001 C CNN
F 3 "~" H 5600 2900 50  0001 C CNN
	1    5600 2900
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR01
U 1 1 612739BF
P 5600 3550
F 0 "#PWR01" H 5600 3300 50  0001 C CNN
F 1 "GND" H 5605 3377 50  0000 C CNN
F 2 "" H 5600 3550 50  0001 C CNN
F 3 "" H 5600 3550 50  0001 C CNN
	1    5600 3550
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R2
U 1 1 6127C3C2
P 6500 2100
F 0 "R2" V 6295 2100 50  0000 C CNN
F 1 "R_US" V 6386 2100 50  0000 C CNN
F 2 "" V 6540 2090 50  0001 C CNN
F 3 "~" H 6500 2100 50  0001 C CNN
	1    6500 2100
	0    1    1    0   
$EndComp
Wire Wire Line
	3050 2100 3350 2100
Wire Wire Line
	3650 2100 4050 2100
Wire Wire Line
	4350 2100 4800 2100
Wire Wire Line
	4800 2450 4800 2700
Wire Wire Line
	4800 3100 4800 3550
Wire Wire Line
	4800 3550 5600 3550
Wire Wire Line
	5600 3550 5600 3050
Connection ~ 5600 3550
Wire Wire Line
	5600 2750 5600 2100
Wire Wire Line
	5600 2100 6350 2100
Wire Wire Line
	4800 2100 5600 2100
Connection ~ 4800 2100
Connection ~ 5600 2100
Wire Wire Line
	6650 2100 7150 2100
Wire Wire Line
	7450 2100 7800 2100
Text Notes 3900 4550 0    50   ~ 0
A diode attenuator attenuates the signal by exploiting the dynamic AC resistance\nvarying inversely prportion to the diode/LED current.\nThere is an exponential relationship between the input voltage and output current.\n\nC1 and C3 acts as decoupling capacitors.\nR1 and R2 provides a current limiter.\nR3 is used as an adjustor to the diode D1 to modulate the dynamic resistance.\nThe battery is sed to provide the voltage source.
$EndSCHEMATC
