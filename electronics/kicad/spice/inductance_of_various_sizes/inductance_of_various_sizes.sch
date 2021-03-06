EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Inductances of Various Sizes"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L pspice:INDUCTOR L1
U 1 1 5E94A4D4
P 7200 2400
F 0 "L1" V 7154 2478 50  0000 L CNN
F 1 "1" V 7245 2478 50  0000 L CNN
F 2 "" H 7200 2400 50  0001 C CNN
F 3 "~" H 7200 2400 50  0001 C CNN
	1    7200 2400
	0    1    1    0   
$EndComp
$Comp
L pspice:INDUCTOR L2
U 1 1 5E94B948
P 7150 3750
F 0 "L2" V 7104 3828 50  0000 L CNN
F 1 "5" V 7195 3828 50  0000 L CNN
F 2 "" H 7150 3750 50  0001 C CNN
F 3 "~" H 7150 3750 50  0001 C CNN
	1    7150 3750
	0    1    1    0   
$EndComp
$Comp
L pspice:INDUCTOR L3
U 1 1 5E94BD26
P 7150 5100
F 0 "L3" V 7104 5178 50  0000 L CNN
F 1 "10" V 7195 5178 50  0000 L CNN
F 2 "" H 7150 5100 50  0001 C CNN
F 3 "~" H 7150 5100 50  0001 C CNN
	1    7150 5100
	0    1    1    0   
$EndComp
$Comp
L pspice:R R1
U 1 1 5E94C7E5
P 6300 2150
F 0 "R1" V 6095 2150 50  0000 C CNN
F 1 "100" V 6186 2150 50  0000 C CNN
F 2 "" H 6300 2150 50  0001 C CNN
F 3 "~" H 6300 2150 50  0001 C CNN
	1    6300 2150
	0    1    1    0   
$EndComp
$Comp
L pspice:R R2
U 1 1 5E94E717
P 6250 3500
F 0 "R2" V 6045 3500 50  0000 C CNN
F 1 "100" V 6136 3500 50  0000 C CNN
F 2 "" H 6250 3500 50  0001 C CNN
F 3 "~" H 6250 3500 50  0001 C CNN
	1    6250 3500
	0    1    1    0   
$EndComp
$Comp
L pspice:R R3
U 1 1 5E94ED61
P 6300 4850
F 0 "R3" V 6095 4850 50  0000 C CNN
F 1 "100" V 6186 4850 50  0000 C CNN
F 2 "" H 6300 4850 50  0001 C CNN
F 3 "~" H 6300 4850 50  0001 C CNN
	1    6300 4850
	0    1    1    0   
$EndComp
Wire Wire Line
	6550 2150 7200 2150
$Comp
L pspice:0 #GND?
U 1 1 5E952118
P 7200 2900
F 0 "#GND?" H 7200 2800 50  0001 C CNN
F 1 "0" H 7200 2989 50  0000 C CNN
F 2 "" H 7200 2900 50  0001 C CNN
F 3 "~" H 7200 2900 50  0001 C CNN
	1    7200 2900
	1    0    0    -1  
$EndComp
$Comp
L pspice:0 #GND?
U 1 1 5E952334
P 7150 4250
F 0 "#GND?" H 7150 4150 50  0001 C CNN
F 1 "0" H 7150 4339 50  0000 C CNN
F 2 "" H 7150 4250 50  0001 C CNN
F 3 "~" H 7150 4250 50  0001 C CNN
	1    7150 4250
	1    0    0    -1  
$EndComp
$Comp
L pspice:0 #GND?
U 1 1 5E9526EF
P 7150 5600
F 0 "#GND?" H 7150 5500 50  0001 C CNN
F 1 "0" H 7150 5689 50  0000 C CNN
F 2 "" H 7150 5600 50  0001 C CNN
F 3 "~" H 7150 5600 50  0001 C CNN
	1    7150 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 2650 7200 2900
Wire Wire Line
	6500 3500 7150 3500
Wire Wire Line
	7150 4000 7150 4250
Wire Wire Line
	6550 4850 7150 4850
Wire Wire Line
	7150 5350 7150 5600
Text GLabel 5950 2150 0    50   Input ~ 0
in
Text GLabel 5900 3500 0    50   Input ~ 0
in
Text GLabel 5950 4850 0    50   Input ~ 0
in
Wire Wire Line
	5950 2150 6050 2150
Wire Wire Line
	5900 3500 6000 3500
Wire Wire Line
	5950 4850 6050 4850
Text Notes 4550 4200 0    50   ~ 0
.tran 0.01 5 0
$Comp
L pspice:ISOURCE I1
U 1 1 5E973D0F
P 4750 3100
F 0 "I1" H 4980 3146 50  0000 L CNN
F 1 "SIN(0, 5, 10, 0, 0, 0)" H 4980 3055 50  0000 L CNN
F 2 "" H 4750 3100 50  0001 C CNN
F 3 "~" H 4750 3100 50  0001 C CNN
	1    4750 3100
	1    0    0    -1  
$EndComp
$Comp
L pspice:0 #GND?
U 1 1 5E97447E
P 4750 3650
F 0 "#GND?" H 4750 3550 50  0001 C CNN
F 1 "0" H 4750 3739 50  0000 C CNN
F 2 "" H 4750 3650 50  0001 C CNN
F 3 "~" H 4750 3650 50  0001 C CNN
	1    4750 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 3500 4750 3650
Text GLabel 4750 2600 1    50   Input ~ 0
in
Wire Wire Line
	4750 2700 4750 2600
$EndSCHEMATC
