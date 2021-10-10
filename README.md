Origin
------
Blitsort is a rotate merge sort based on [quadsort](https://github.com/scandum/quadsort).

Visualization
-------------
In the visualization below nine tests are performed on 256 elements.

1. Random order
2. Ascending order
3. Ascending Saw
4. Generic random order
5. Descending order
6. Descending Saw
7. Random tail
8. Random half
9. Ascending tiles.

The upper half shows the swap memory of 32 elements, and the bottom half shows the main memory.
Colors are used to differentiate various operations. Parity merges are in orange. Rotations are in yellow and violet.

[![blitsort benchmark](/images/blitsort.gif)](https://www.youtube.com/watch?v=3ibxXQ1KCbI)

Quad swap
---------
Blitsort uses the same quad swap as quadsort. It is a routine that creates blocks of 32 sorted elements using 31 comparisons for in-order and reverse-order data, and aproximately 160 comparisons for random data.

A block of 4 elements is created with a decision tree, a block of 4 is turned into a block of 8 with a binary decision tree, and four blocks of 8 are turned into a block of 32 using a branchless parity merge.

Quad merge
----------
Blitsort will quad merge (also known as ping-pong merge) using branchless parity merges until the available stack memory is exhausted.

Rotate merge sort
-----------------
A rotate merge sort uses rotations to partition two sorted arrays until they're small enough to be merged using auxiliary memory. Blitsort does so by taking the center element of the first array, using a binary search to find all elements smaller than the center element in the second array, and performing an array rotation. It does so recursively until a partition becomes small enough to be merged.

Monobound binary search
-----------------------
Blitsort uses a [monobound binary search](https://github.com/scandum/binary_search), which is up to two times faster than the binary search in general use.

Trinity rotation
----------------
Blitsort uses a [trinity rotation](https://github.com/scandum/rotate), a new and significantly faster array rotation algorithm.

Memory
------
By default blitsort uses 512 elements worth of stack memory.

The minimum memory requirement for blitsort is 32 elements of stack memory, it can be configured to use sqrt(n) memory.

Blitsort rotate merges recursively, requiring an additional log(n) memory. It's possible to make this O(1) through the implementation of a stack.

There is currently no clear consensus on what constitutes as an in-place sort, it boils down to what someone considers a small enough memory footprint to be considered negligable. This typically ranges from the size of a cache line to the size of the L1 cache.

Performance
-----------
Blitsort has exceptional performance due to the quad swap, monobound binary search, and trinity rotation. It is likely the fastest in-place stable sort written so far and is about 15% faster than [octosort](https://github.com/scandum/octosort), which is a block merge sort.

Blitsort's performance is similar to that of quadsort as long as the auxiliary memory is greater or equal to the square root of the array being sorted, which comes out at 262,144 elements with the default stack of 512 elements. Performance on larger arrays will degrade slowly.

Data Types
----------
Blitsort supports long doubles and 8, 16, 32, and 64 bit data types. By using 32 or 64 bit pointers it's possible to sort any other data type, like strings. Custom data sizes can be added in blitsort.h.

Interface
---------
The interface is the same one as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Big O
-----
```cobol
                 ┌───────────────────────┐┌───────────────────────┐
                 │comparisons            ││swap memory            │
┌───────────────┐├───────┬───────┬───────┤├───────┬───────┬───────┤┌──────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min    │avg    │max    ││stable││partition││adaptive │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│blitsort       ││n      │n log n│n log n││1      │1      │1      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│mergesort      ││n log n│n log n│n log n││n      │n      │n      ││yes   ││no       ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│timsort        ││n      │n log n│n log n││n      │n      │n      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quicksort      ││n      │n log n│n²     ││1      │1      │1      ││no    ││yes      ││no       │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘
```

Benchmark: blitsort vs std::stable_sort vs gfx::timsort
-------------------------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort). The source code
was compiled using `g++ -O3 -w -fpermissive bench.c`. Stablesort is g++'s std:stable_sort function.

The graph shows the relative performance on 100,000 32 bit integers. A table with the best and
average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph1.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |   100000 |   32 | 0.006074 | 0.006098 |         1 |     100 |     random order |
|  blitsort |   100000 |   32 | 0.004407 | 0.004430 |         1 |     100 |     random order |
|   timsort |   100000 |   32 | 0.007633 | 0.007670 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003895 | 0.003917 |         1 |     100 |     random % 100 |
|  blitsort |   100000 |   32 | 0.002346 | 0.002355 |         1 |     100 |     random % 100 |
|   timsort |   100000 |   32 | 0.005585 | 0.005617 |         1 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000683 | 0.000700 |         1 |     100 |        ascending |
|  blitsort |   100000 |   32 | 0.000073 | 0.000073 |         1 |     100 |        ascending |
|   timsort |   100000 |   32 | 0.000045 | 0.000046 |         1 |     100 |        ascending |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001379 | 0.001441 |         1 |     100 |    ascending saw |
|  blitsort |   100000 |   32 | 0.001114 | 0.001122 |         1 |     100 |    ascending saw |
|   timsort |   100000 |   32 | 0.000854 | 0.000861 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000819 | 0.000838 |         1 |     100 |       pipe organ |
|  blitsort |   100000 |   32 | 0.000363 | 0.000364 |         1 |     100 |       pipe organ |
|   timsort |   100000 |   32 | 0.000175 | 0.000178 |         1 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000899 | 0.000920 |         1 |     100 |       descending |
|  blitsort |   100000 |   32 | 0.000053 | 0.000053 |         1 |     100 |       descending |
|   timsort |   100000 |   32 | 0.000101 | 0.000103 |         1 |     100 |       descending |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000963 | 0.000969 |         1 |     100 |   descending saw |
|  blitsort |   100000 |   32 | 0.000633 | 0.000637 |         1 |     100 |   descending saw |
|   timsort |   100000 |   32 | 0.000312 | 0.000315 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002063 | 0.002121 |         1 |     100 |      random tail |
|  blitsort |   100000 |   32 | 0.001353 | 0.001360 |         1 |     100 |      random tail |
|   timsort |   100000 |   32 | 0.002005 | 0.002015 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003552 | 0.003571 |         1 |     100 |      random half |
|  blitsort |   100000 |   32 | 0.002532 | 0.002546 |         1 |     100 |      random half |
|   timsort |   100000 |   32 | 0.004030 | 0.004051 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000962 | 0.000980 |         1 |     100 |  ascending tiles |
|  blitsort |   100000 |   32 | 0.000838 | 0.000841 |         1 |     100 |  ascending tiles |
|   timsort |   100000 |   32 | 0.000866 | 0.000946 |         1 |     100 |  ascending tiles |

</details>

The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)
using the [wolfsort benchmark](https://github.com/scandum/wolfsort).
The source code was compiled using `g++ -O3 -w -fpermissive bench.c`. It measures the performance on random data with array sizes
ranging from 8 to 524288. The benchmark is weighted, meaning the number of repetitions
halves each time the number of items doubles. A table with the best and average time in seconds can be uncollapsed below the bar graph.

![Graph](/images/graph2.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |        8 |   32 | 0.006141 | 0.006181 |     65536 |     100 |         random 8 |
|  blitsort |        8 |   32 | 0.002758 | 0.002779 |     65536 |     100 |         random 8 |
|   timsort |        8 |   32 | 0.006440 | 0.006640 |     65536 |     100 |         random 8 |
|           |          |      |          |          |           |         |                  |
|stablesort |       32 |   32 | 0.009546 | 0.009650 |     16384 |     100 |        random 32 |
|  blitsort |       32 |   32 | 0.004483 | 0.004539 |     16384 |     100 |        random 32 |
|   timsort |       32 |   32 | 0.012943 | 0.013052 |     16384 |     100 |        random 32 |
|           |          |      |          |          |           |         |                  |
|stablesort |      128 |   32 | 0.013275 | 0.013338 |      4096 |     100 |       random 128 |
|  blitsort |      128 |   32 | 0.005507 | 0.005540 |      4096 |     100 |       random 128 |
|   timsort |      128 |   32 | 0.019890 | 0.019977 |      4096 |     100 |       random 128 |
|           |          |      |          |          |           |         |                  |
|stablesort |      512 |   32 | 0.017126 | 0.017193 |      1024 |     100 |       random 512 |
|  blitsort |      512 |   32 | 0.007016 | 0.007091 |      1024 |     100 |       random 512 |
|   timsort |      512 |   32 | 0.024587 | 0.024727 |      1024 |     100 |       random 512 |
|           |          |      |          |          |           |         |                  |
|stablesort |     2048 |   32 | 0.020837 | 0.020913 |       256 |     100 |      random 2048 |
|  blitsort |     2048 |   32 | 0.010782 | 0.010841 |       256 |     100 |      random 2048 |
|   timsort |     2048 |   32 | 0.028993 | 0.029106 |       256 |     100 |      random 2048 |
|           |          |      |          |          |           |         |                  |
|stablesort |     8192 |   32 | 0.024646 | 0.024721 |        64 |     100 |      random 8192 |
|  blitsort |     8192 |   32 | 0.014863 | 0.014938 |        64 |     100 |      random 8192 |
|   timsort |     8192 |   32 | 0.033164 | 0.033287 |        64 |     100 |      random 8192 |
|           |          |      |          |          |           |         |                  |
|stablesort |    32768 |   32 | 0.028555 | 0.028642 |        16 |     100 |     random 32768 |
|  blitsort |    32768 |   32 | 0.019056 | 0.019100 |        16 |     100 |     random 32768 |
|   timsort |    32768 |   32 | 0.037455 | 0.037561 |        16 |     100 |     random 32768 |
|           |          |      |          |          |           |         |                  |
|stablesort |   131072 |   32 | 0.032554 | 0.032656 |         4 |     100 |    random 131072 |
|  blitsort |   131072 |   32 | 0.023500 | 0.023555 |         4 |     100 |    random 131072 |
|   timsort |   131072 |   32 | 0.041684 | 0.041808 |         4 |     100 |    random 131072 |
|           |          |      |          |          |           |         |                  |
|stablesort |   524288 |   32 | 0.036549 | 0.036706 |         1 |     100 |    random 524288 |
|  blitsort |   524288 |   32 | 0.028150 | 0.028298 |         1 |     100 |    random 524288 |
|   timsort |   524288 |   32 | 0.046034 | 0.046175 |         1 |     100 |    random 524288 |

</details>

Benchmark: blitsort vs qsort (mergesort)
----------------------------------------
The following benchmark was on WSL 2 gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04).
The source code was compiled using gcc -O3 bench.c. The graph shows the relative performance on
100,000 32 bit integers. A table with the best and average time in seconds can be uncollapsed
below the bar graph.

![Graph](/images/graph4.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   32 | 0.008467 | 0.008570 |   1536634 |     100 |     random order |
|  blitsort |   100000 |   32 | 0.005823 | 0.005852 |   1655100 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002020 | 0.002094 |    815024 |     100 |  ascending order |
|  blitsort |   100000 |   32 | 0.000197 | 0.000198 |     99999 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002819 | 0.002893 |    915019 |     100 |    ascending saw |
|  blitsort |   100000 |   32 | 0.001524 | 0.001538 |    388029 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006406 | 0.006548 |   1532339 |     100 |    generic order |
|  blitsort |   100000 |   32 | 0.003740 | 0.003778 |   1590862 |     100 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002451 | 0.002482 |    853904 |     100 | descending order |
|  blitsort |   100000 |   32 | 0.000157 | 0.000157 |     99999 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003222 | 0.003284 |    953892 |     100 |   descending saw |
|  blitsort |   100000 |   32 | 0.001509 | 0.001526 |    400000 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003731 | 0.003809 |   1012028 |     100 |      random tail |
|  blitsort |   100000 |   32 | 0.001872 | 0.001886 |    577145 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005446 | 0.005593 |   1200835 |     100 |      random half |
|  blitsort |   100000 |   32 | 0.003382 | 0.003404 |    972502 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004079 | 0.004528 |   1209200 |     100 |  ascending tiles |
|  blitsort |   100000 |   32 | 0.002711 | 0.002734 |    833395 |     100 |  ascending tiles |

</details>
