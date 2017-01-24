This is release 1.1 of the eTPU AC Motor Control function set - set4.
The package contains the binary image of the eTPU code for the CPU to use,
the source code and the interface files for the function APIs.

Files included in this package:
===============================

etpu_set4\readme.txt     - This file.
etpu_set4\etpu_set4.h    - Image of eTPU code for host CPU (generate by
                           eTPU compiler).
etpu_set4\eTPUmake.bat   - A "makefile" used to build the code.
etpu_set4\etpuc_set4.cod - Debug information for set4 functions.
etpu_set4\etpuc_set4.lst - Listing file.

eTPU code
---------
etpu_set4\etpuc_set4.c    - The set4 master file.
etpu_set4\etpuc_set4.h    - The set4 master file - definition of function
                            numbers and code memory size.
etpu_set4\etpuc.h         - Standard include file for eTPU code from ByteCraft.
etpu_set4\etpuc_common.h  - Standard include file for eTPU code from ByteCraft.
etpu_set4\etpuc_mc.h      - The eTPU set4 common definitions.
etpu_set4\etpuc_gpio.c    - The eTPU GPIO function.
etpu_set4\etpuc_pwmm.h    - The eTPU motor control PWM common definitions.
etpu_set4\etpuc_pwmmac.c  - The eTPU PWMMAC function - SVMSTD and PWMICT 
                            modulations only.
etpu_set4\etpuc_pwmf.c    - The eTPU PWMF function - no HALF CYCLE mode.
etpu_set4\etpuc_qd.c      - The eTPU QD, QDHOME and QDINDEX functions.
etpu_set4\etpuc_rslv.c    - The eTPU RSLV function.
etpu_set4\etpuc_acimvhz.c - The eTPU ACIMVHZ function.
etpu_set4\etpuc_acimvc.c  - The eTPU ACIMVC function.
etpu_set4\etpuc_pmsmvc.c  - The eTPU PMSMVC function.
etpu_set4\etpuc_sc.c      - The SC function - QD mode only.
etpu_set4\etpuc_bc.c      - The eTPU BC function.
etpu_set4\etpuc_asac.c    - The ASAC function.
etpu_set4\etpuc_mc_ctrl.h - The eTPU motor control controllers header file.
etpu_set4\etpuc_mc_ctrl.c - The eTPU motor control controllers - PID routine.
etpu_set4\etpuc_mc_sin.h  - The eTPU sine and look-up-table header file.
etpu_set4\etpuc_mc_sin.c  - The eTPU sine and look-up-table routine.
etpu_set4\etpuc_mc_tran.c - The eTPU vector control transformations header file.
etpu_set4\etpuc_mc_tran.h - The eTPU vector control transformation routines.
etpu_set4\etpuc_mc_elim.c - The eTPU ripple elimination routine.
etpu_set4\etpuc_mc_ramp.h - The eTPU motor control ramp routine definitions.
etpu_set4\etpuc_mc_ramp.c - The eTPU motor control ramp implementation.
etpu_set4\etpuc_mc_util.c - The eTPU math routines.


CPU interface files
-------------------
etpu_set4\cpu\etpu_gpio_auto.h    - GPIO function interface information.
etpu_set4\cpu\etpu_pwmmac_auto.h  - PWMMAC function interface information.
etpu_set4\cpu\etpu_pwmf_auto.h    - PWMF function interface information.
etpu_set4\cpu\etpu_acimvhz_auto.h - ACIMVHZ function interface information.
etpu_set4\cpu\etpu_acimvc_auto.h  - ACIMVC function interface information.
etpu_set4\cpu\etpu_pmsmvc_auto.h  - PMSMVC function interface information.
etpu_set4\cpu\etpu_qd_auto.h      - QD function interface information.
etpu_set4\cpu\etpu_rslv_auto.h    - RSLV function interface information.
etpu_set4\cpu\etpu_sc_auto.h      - SC function interface information.
etpu_set4\cpu\etpu_bc_auto.h      - BC function interface information.
etpu_set4\cpu\etpu_asac_auto.h    - ASAC function interface information.
etpu_set4\cpu\etpu_mc_ctrl_auto.h - PID Controller interface info.
etpu_set4\cpu\etpu_mc_sin_auto.h  - Sine and look-up-table interface info.


Code Memory Occupation:
=======================
All size are in bytes. Version 1.0.7 of the ByteCraft eTPU compiler was used.

 Nickname    |Version| Start |  End  |Code Size|Entry Table| Total |
-------------+-------+-------+-------+---------+-----------+-------+
GPIO         |  1.2  | 0x0378| 0x0410|    152  |      64   |   216 |
QD           |  1.1  | 0x0410| 0x07F4|    996  |      64   | 1,060 |
QDHOME       |  1.0  | 0x07F4| 0x0824|     48  |      64   |   112 |
QDINDEX      |  1.0  | 0x0824| 0x0950|    300  |      64   |   364 |
SC           |  1.4  | 0x0A54| 0x0CF4|    672  |      64   |   736 |
BC           |  1.0  | 0x0CF4| 0x0E68|    372  |      64   |   436 |
PWMMAC       |  0.6  | 0x0FEC| 0x163C|  1,296  |      64   | 1,360 |
PWMF         |  1.2  | 0x163C| 0x1860|    548  |      64   |   612 |
ASAC         |  0.5  | 0x1860| 0x1D80|  1,184  |      64   | 1,248 |
ACIMVHZ      |  1.2  | 0x1DD8| 0x1F60|    392  |      64   |   456 |
PMSMVC       |  1.3  | 0x1FC8| 0x22AC|    740  |      64   |   804 |
ACIMVC       |  1.1  | 0x22AC| 0x2850|  1,444  |      64   | 1,508 |
RSLV         |  1.2  | 0x2850| 0x2E88|  1,592  |      64   | 1,656 |
-------------+-------+-------+-------+---------+-----------+-------+
Global_Error |       | 0x0340| 0x0378|     56  |       0   |    56 |
mc_ctrl_pid  |  1.2  | 0x0950| 0x0A24|    212  |       0   |   212 |
mc_ramp      |  1.0  | 0x0A24| 0x0A54|     48  |       0   |    48 |
mc_sin       |  1.0  | 0x0E68| 0x0EE4|    124  |       0   |   124 |
mc_util      |  1.0  | 0x0EE4| 0x0FEC|    264  |       0   |   264 |
mc_ripple_elim| 1.1  | 0x1D80| 0x1DD8|     88  |       0   |    88 |
mc_tran      |  1.0  | 0x1F60| 0x1FC8|    104  |       0   |   104 |
-------------+-------+-------+-------+---------+-----------+-------+
                                     TOTAL Code Memory used: 11,912

Global_Error is a subroutine used by all the functions to provide error
information to the host CPU.
mc_ctrl_pid is a subroutine used by SC, ACIMVC and PMSMVC to provide PID 
controller calculations.
mc_sin includes subroutines used by ACIMVHZ, ACIMVC and PMSMVC to provide 
look-up-table calculations, used for sin and cosine value calculation.
mc_util contains the eTPU implementation of several math utilities.
mc_tran includes subroutines used by ACIMVC and PMSMVC to provide Park and Clark 
transformations.
mc_ripple_elim is a subroutine used by ACIMVHZ, ACIMVC and PMSMVC to provide 
DC-bus ripple elimition.
mc_ramp is a subroutine used by SC and ACIMVHZ to provide ramp calculation.
To support the eTPU functions the etpu.h (vesion 1.02) and etpuc_common.h
(version 1.0) files from ByteCraft were used.


Performance (not updated for release 1.1):
============

Funct. | Mode of operation              | max eTPU busy time per | eTPU cycles
-------+--------------------------------+------------------------+------------
GPIO   | Input-detection of transitions | one transition         |     26
       | Input-immediate read pin state | one read               |     26
       | Input-period read of pin state | one period             |     30
       | Output-immediate set pin state | one set                |      8
-------+--------------------------------+------------------------+------------
PWMMAC | modulation: no modulation      | one period             |    340
with   | update: normal                 | period-update time     |     56
PWMF   | number of phases: 1            | Minumim update time    |    312
phases | phases type: single channels   | Longest thread         |    188
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |    466
       | update: normal                 | period-update time     |     84
       | number of phases: 1            | Minumim update time    |    410
       | phases type: compl. pairs      | Longest thread         |    190
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |    586
       | update: normal                 | period-update time     |     84
       | number of phases: 2            | Minumim update time    |    530
       | phases type: single channels   | Longest thread         |    310
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |    836
       | update: normal                 | period-update time     |    140
       | number of phases: 2            | Minumim update time    |    724
       | phases type: compl. pairs      | Longest thread         |    312
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |    832
       | update: normal                 | period-update time     |    112
       | number of phases: 3            | Minumim update time    |    748
       | phases type: single channels   | Longest thread         |    432
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |   1206
       | update: normal                 | period-update time     |    196
       | number of phases: 3            | Minumim update time    |   1038
       | phases type: compl. pairs      | Longest thread         |    434
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |    620
       | update: half cycle             | half period-update time|     56
       | number of phases: 1            | Minumim update time    |    388
       | phases type: single channels   | Longest thread         |    196
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |    816
       | update: half cycle             | half period-update time|     84
       | number of phases: 1            | Minumim update time    |    554
       | phases type: compl. pairs      | Longest thread         |    198
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |   1048
       | update: half cycle             | half period-update time|     84
       | number of phases: 2            | Minumim update time    |    678
       | phases type: single channels   | Longest thread         |    322
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |   1436
       | update: half cycle             | half period-update time|    140
       | number of phases: 2            | Minumim update time    |   1008
       | phases type: compl. pairs      | Longest thread         |    324
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |   1476
       | update: half cycle             | half period-update time|    112
       | number of phases: 3            | Minumim update time    |    968
       | phases type: single channels   | Longest thread         |    448
       +--------------------------------+------------------------+------------
       | modulation: no modulation      | one period             |   2056
       | update: half cycle             | half period-update time|    196
       | number of phases: 3            | Minumim update time    |   1462
       | phases type: compl. pairs      | Longest thread         |    450
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |    360
       | update: normal                 | period-update time     |     76
       | number of phases: 1            | Minumim update time    |    332
       | phases type: single channels   | Longest thread         |    188
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |    486
       | update: normal                 | period-update time     |    104
       | number of phases: 1            | Minumim update time    |    430
       | phases type: compl. pairs      | Longest thread         |    190
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |    606
       | update: normal                 | period-update time     |    104
       | number of phases: 2            | Minumim update time    |    550
       | phases type: single channels   | Longest thread         |    310
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |    856
       | update: normal                 | period-update time     |    160
       | number of phases: 2            | Minumim update time    |    744
       | phases type: compl. pairs      | Longest thread         |    312
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |    852
       | update: normal                 | period-update time     |    132
       | number of phases: 3            | Minumim update time    |    768
       | phases type: single channels   | Longest thread         |    432
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |   1226
       | update: normal                 | period-update time     |    216
       | number of phases: 3            | Minumim update time    |   1058
       | phases type: compl. pairs      | Longest thread         |    434
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |    660
       | update: half cycle             | half period-update time|     76
       | number of phases: 1            | Minumim update time    |    408
       | phases type: single channels   | Longest thread         |    196
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |    856
       | update: half cycle             | half period-update time|    104
       | number of phases: 1            | Minumim update time    |    574
       | phases type: compl. pairs      | Longest thread         |    198
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |   1088
       | update: half cycle             | half period-update time|    104
       | number of phases: 2            | Minumim update time    |    698
       | phases type: single channels   | Longest thread         |    322
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |   1476
       | update: half cycle             | half period-update time|    160
       | number of phases: 2            | Minumim update time    |   1028
       | phases type: compl. pairs      | Longest thread         |    324
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |   1516
       | update: half cycle             | half period-update time|    132
       | number of phases: 3            | Minumim update time    |    988
       | phases type: single channels   | Longest thread         |    448
       +--------------------------------+------------------------+------------
       | modulation: voltage sign/unsign| one period             |   2096
       | update: half cycle             | half period-update time|    216
       | number of phases: 3            | Minumim update time    |   1482
       | phases type: compl. pairs      | Longest thread         |    450
       +--------------------------------+------------------------+------------
       | modulation: SVM Stand./U0n/U7n | one period             |    966
       | update: normal                 | period-update time     |    278
       | number of phases: 3            | Minumim update time    |    882
       | phases type: single channels   | Longest thread         |    400
       +--------------------------------+------------------------+------------
       | modulation: SVM Stand./U0n/U7n | one period             |   1340
       | update: normal                 | period-update time     |    362
       | number of phases: 3            | Minumim update time    |   1172
       | phases type: compl. pairs      | Longest thread         |    402
       +--------------------------------+------------------------+------------
       | modulation: SVM Stand./U0n/U7n | one period             |   1812
       | update: half cycle             | period-update time     |    278
       | number of phases: 3            | Minumim update time    |   1134
       | phases type: single channels   | Longest thread         |    448
       +--------------------------------+------------------------+------------
       | modulation: SVM Stand./U0n/U7n | one period             |   2392
       | update: half cycle             | period-update time     |    362
       | number of phases: 3            | Minumim update time    |   1628
       | phases type: compl. pairs      | Longest thread         |    450
       +--------------------------------+------------------------+------------
       | modulation: SVM ICT            | one period             |    946
       | update: normal                 | period-update time     |    262
       | number of phases: 3            | Minumim update time    |    862
       | phases type: single channels   | Longest thread         |    396
       +--------------------------------+------------------------+------------
       | modulation: SVM ICT            | one period             |   1320
       | update: normal                 | period-update time     |    346
       | number of phases: 3            | Minumim update time    |   1152
       | phases type: compl. pairs      | Longest thread         |    398
       +--------------------------------+------------------------+------------
       | modulation: SVM ICT            | one period             |   1776
       | update: half cycle             | period-update time     |    262
       | number of phases: 3            | Minumim update time    |   1118
       | phases type: single channels   | Longest thread         |    448
       +--------------------------------+------------------------+------------
       | modulation: SVM ICT            | one period             |   2356
       | update: half cycle             | period-update time     |    346
       | number of phases: 3            | Minumim update time    |   1612
       | phases type: compl. pairs      | Longest thread         |    450
       +--------------------------------+------------------------+------------
       | modulation: sine table         | one period             |    956
       | update: normal                 | period-update time     |    272
       | number of phases: 3            | Minumim update time    |    872
       | phases type: single channels   | Longest thread         |    396
       +--------------------------------+------------------------+------------
       | modulation: sine table         | one period             |   1330
       | update: normal                 | period-update time     |    356
       | number of phases: 3            | Minumim update time    |   1162
       | phases type: compl. pairs      | Longest thread         |    398
       +--------------------------------+------------------------+------------
       | modulation: sine table         | one period             |   1796
       | update: half cycle             | period-update time     |    272
       | number of phases: 3            | Minumim update time    |   1128
       | phases type: single channels   | Longest thread         |    448
       +--------------------------------+------------------------+------------
       | modulation: sine table         | one period             |   2376
       | update: half cycle             | period-update time     |    356
       | number of phases: 3            | Minumim update time    |   1622
       | phases type: compl. pairs      | Longest thread         |    450
       +--------------------------------+------------------------+------------
       | modulation: user table         | one period             |   1010
       | update: normal                 | period-update time     |    326
       | number of phases: 3            | Minumim update time    |    926
       | phases type: single channels   | Longest thread         |    398
       +--------------------------------+------------------------+------------
       | modulation: user table         | one period             |   1384
       | update: normal                 | period-update time     |    410
       | number of phases: 3            | Minumim update time    |   1216
       | phases type: compl. pairs      | Longest thread         |    398
       +--------------------------------+------------------------+------------
       | modulation: user table         | one period             |   1904
       | update: half cycle             | period-update time     |    326
       | number of phases: 3            | Minumim update time    |   1182
       | phases type: single channels   | Longest thread         |    448
       +--------------------------------+------------------------+------------
       | modulation: user table         | one period             |   2484
       | update: half cycle             | period-update time     |    410
       | number of phases: 3            | Minumim update time    |   1676
       | phases type: compl. pairs      | Longest thread         |    450
-------+--------------------------------+------------------------+------------
ACIMVHZ| Master mode, Speed Ramp OFF,   |                        |    
       | Alpha-Beta mode                | one update             |    290
       +--------------------------------+------------------------+------------
       | Master mode, Speed Ramp OFF,   |                        |    
       | Amplitude-Angle mode           | one update             |    154
       +--------------------------------+------------------------+------------
       | Master mode, Speed Ramp ON,    |                        |    
       | Alpha-Beta mode                | one update             |    322
       +--------------------------------+------------------------+------------
       | Master mode, Speed Ramp ON,    |                        |    
       | Amplitude-Angle mode           | one update             |    186
       +--------------------------------+------------------------+------------
       | Slave mode, Speed Ramp OFF,    |                        |    
       | Alpha-Beta mode                | one update             |    282
       +--------------------------------+------------------------+------------
       | Slave mode, Speed Ramp OFF,    |                        |    
       | Amplitude-Angle mode           | one update             |    146
       +--------------------------------+------------------------+------------
       | Slave mode, Speed Ramp ON,     |                        |    
       | Alpha-Beta mode                | one update             |    310
       +--------------------------------+------------------------+------------
       | Slave mode, Speed Ramp ON,     |                        |    
       | Amplitude-Angle mode           | one update             |    174
-------+--------------------------------+------------------------+------------
ACIMVC | Master mode,                   | one update             |   1304
       | Circle limitation OFF          | Longest thread         |    584
       +--------------------------------+------------------------+------------
       | Master mode,                   | one update             |   1550
       | Circle limitation ON           | Longest thread         |    584
       +--------------------------------+------------------------+------------
       | Slave mode,                    | one update             |   1292
       | Circle limitation OFF          | Longest thread         |    584
       +--------------------------------+------------------------+------------
       | Slave mode,                    | one update             |   1538
       | Circle limitation ON           | Longest thread         |    584
-------+--------------------------------+------------------------+------------
PMSMVC | Master mode,                   | one update             |    766
       | Circle limitation OFF          | Longest thread         |    546
       +--------------------------------+------------------------+------------
       | Master mode,                   | one update             |   1010
       | Circle limitation ON           | Longest thread         |    546
       +--------------------------------+------------------------+------------
       | Slave mode,                    | one update             |    754
       | Circle limitation OFF          | Longest thread         |    546
       +--------------------------------+------------------------+------------
       | Slave mode,                    | one update             |    998
       | Circle limitation ON           | Longest thread         |    546
-------+--------------------------------+------------------------+------------
QD     | Slow   - Phase A + Phase B     | one encoder pulse      |    622
       | Slow   - Phase A + B and Index | one encoder pulse      |    718
       |                                | Longest thread         |    218
       | Normal - Phase A + Phase B     | one encoder pulse      |    660
       | Normal - Phase A + B and Index | one encoder pulse      |    720
       |                                | Longest thread         |    232
       | Fast   - Phase A + Phase B     | one encoder pulse      |    246
       | Fast   - Phase A + B and Index | one encoder pulse      |    310
       |                                | Longest thread         |    246
-------+--------------------------------+------------------------+------------
SC     | Master mode, QD, PI controller | one update             |    322
       | Master mode, QD, PID controller| one update             |    332
       | Slave mode, QD, PI controller  | one update             |    310
       | Slave mode, QD, PID controller | one update             |    320
-------+--------------------------------+------------------------+------------
BC     | Master mode, ON/OFF switching  | one update             |     58
       | Slave mode,  ON/OFF switching  | one update             |     46
       | Slave mode,  PWM switching     | one update             |     64
       | Slave mode,  PWM switching     | one PWM edge           |     20
-------+--------------------------------+------------------------+------------
ASAC   | command queue is not generated | one 1st ASAC edge      |     54
       +--------------------------------+------------------------+------------
       | command queue is generated     | one 1st ASAC edge      |    112
       +--------------------------------+------------------------+------------
       | sample D (DC_BUS voltage)      |                        |     
       | processed only                 | one 2nd ASAC edge      |     96
       +--------------------------------+------------------------+------------
       | samples A,B,C (phase currents) |                        |     
       | processed only,                |                        |     
       | 3 phase currents measured,     |                        |     
       | DTC calculation is OFF         | one 2nd ASAC edge      |    248
       +--------------------------------+------------------------+------------
       | samples A,B,C (phase currents) |                        |     
       | processed only,                |                        |     
       | 3 phase currents measured,     |                        |     
       | DTC calculation is ON          | one 2nd ASAC edge      |    304
       +--------------------------------+------------------------+------------
       | samples A,B,C (phase currents) |                        |     
       | processed only,                |                        |     
       | 2 phase currents measured,     |                        |     
       | 3rd phase current is calculated|                        |     
       | DTC calculation is OFF         | one 2nd ASAC edge      |    262 
       +--------------------------------+------------------------+------------
       | samples A,B,C (phase currents) |                        |     
       | processed only,                |                        |     
       | 2 phase currents measured,     |                        |     
       | 3rd phase current is calculated|                        |     
       | DTC calculation is ON          | one 2nd ASAC edge      |    318
       +--------------------------------+------------------------+------------
       | all samples A,B,C,D processed, |                        |     
       | 3 phase currents measured,     |                        |     
       | DTC calculation is OFF         | one 2nd ASAC edge      |    294
       +--------------------------------+------------------------+------------
       | all samples A,B,C,D processed, |                        |     
       | 3 phase currents measured,     |                        |     
       | DTC calculation is ON          | one 2nd ASAC edge      |    350 
       +--------------------------------+------------------------+------------
       | all samples A,B,C,D processed, |                        |     
       | 2 phase currents measured,     |                        |     
       | 3rd phase current is calculated|                        |     
       | DTC calculation is OFF         | one 2nd ASAC edge      |    310 
       +--------------------------------+------------------------+------------
       | all samples A,B,C,D processed, |                        |     
       | 2 phase currents measured,     |                        |     
       | 3rd phase current is calculated|                        |     
       | DTC calculation is ON          | one 2nd ASAC edge      |    364 
-------+--------------------------------+------------------------+------------


HISTORY
=======
0.1 - First release, supporting ACIM V/Hz applications (10-Aug-05).
0.11- Update of PWMMAC and ACIMVHZ - part of AN3000SW (17-Oct-05).
0.2 - Release fitted to 6kB of CODE RAM, supporting PMSM Vector Control 
      applications - part of AN3002SW (17-Oct-05).
0.5 - All set4 functions except ACIMVC, 8 kB of CODE RAM (25-Oct-05).
1.0 - All set4 functions, 10 kB of CODE RAM (07-Apr-06).
1.1 - RSLV function added. ASAC, PMSMVC, SC and PWMF updated. 12 kB of CODE RAM
      (27-Jan-12).
