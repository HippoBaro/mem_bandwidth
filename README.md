# mem_bandwidth
Absurdly over-engineered C++ STREAM-like benchmarking tool

# Sample output

```
Testing using Single threaded serial scheduler:
Kernel    Max (GB/s)    Min (GB/s)  Avr (GB/s)
Fill      16.34359204   4.73745602  11.45581529
Copy      22.29047955   2.62862924  12.25058444
Scale     17.61273655   5.62377623  14.41017294
Sum       17.29768948   1.68461536  10.11201218
Triad     17.56100193   4.15732138  13.89831946
Vsum       9.28089257   5.97476831   7.63142594
Vprod      5.57501985   3.02254614   4.07553524

Testing using Multi threaded parallel scheduler:
Kernel    Max (GB/s)    Min (GB/s)  Avr (GB/s)
Fill      24.84502979   5.58200757  19.47911895
Copy      45.51954837   7.51830350  20.86125857
Scale     23.19925329   7.18768837  14.33155116
Sum       21.94276536   8.33745105  15.24099577
Triad     24.12389746   8.18939972  14.40516221
Vsum      62.67813301  10.35197165  40.68495475
Vprod     42.42819303  25.22434924  35.63278571
```
