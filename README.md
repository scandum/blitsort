Origin
------
Blitsort is an in-place rotate quick/merge sort based on [quadsort](https://github.com/scandum/quadsort) and [fluxsort](https://github.com/scandum/fluxsort).

Visualization
-------------
In the visualization below the upper half shows the swap memory (32 elements) and the bottom half shows the main memory (512 elements). Colors are used to differentiate between various operations. Partitioning is in light and dark blue, rotations are in yellow and violet.

[![blitsort benchmark](/images/blitsort.gif)](https://www.youtube.com/watch?v=WaqkBO_nV7k)

The visualization only shows the sorting of random data, the sorting of ordered data is identical to the visualization on the quadsort github page.

Analyzer
--------
Blitsort uses the same analyzer as fluxsort. The distribution is scanned to determine how much of it is already sorted. Distributions that are fully in order or in reverse-order are handled immediately. Distributions that are mostly sorted are sorted with rotate quadsort, otherwise they're sorted with rotate fluxsort.

Rotate merge sort
-----------------
A rotate merge sort uses rotations to partition two sorted arrays until they're small enough to be merged using auxiliary memory. Blitsort does so by taking the center element of the first array, using a binary search to find all elements smaller than the center element in the second array, and performing an array rotation. It does so recursively until a partition becomes small enough to be merged.

Rotate stable quicksort
-----------------------
A stable rotate quicksort partitions the array in segments using auxiliary memory, the left and right partition of each segment is then moved to the proper location recursively using rotations. On random data it's faster than a rotate mergesort since no binary search is required and because the branchless optimizations are slightly more efficient. 

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

Blitsort partitions recursively, requiring an additional log(n) memory. It's possible to make this O(1) through the implementation of a stack.

There is currently no clear consensus on what constitutes as an in-place sort, it boils down to what someone considers a small enough memory footprint to be considered negligable. This typically ranges from the size of a cache line to the size of the L1 cache.

Performance
-----------
Blitsort has exceptional performance due to the use of trinity / bridge rotations. It is likely the fastest in-place stable sort written so far and is about 15-50% faster than [octosort](https://github.com/scandum/octosort), which is a block merge sort.

Blitsort's performance is similar to that of fluxsort as long as it has sqrt(n) auxiliary memory. Performance on larger arrays degrades steadily but will still beat most traditional sorts at 10 million elements.

To take full advantage of branchless operations the `cmp` macro needs to be uncommented in bench.c, which will double the performance on primitive types.

Data Types
----------
Blitsort supports long doubles and 8, 16, 32, and 64 bit data types. By using 32 or 64 bit pointers it's possible to sort any other data type, like strings. Custom data sizes can be added in blitsort.h. For any practical use where stability matters you'll likely be using pointers however.

Interface
---------
The interface is the same one as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Big O
-----
```
                 ┌───────────────────────┐┌───────────────────────┐
                 │comparisons            ││swap memory            │
┌───────────────┐├───────┬───────┬───────┤├───────┬───────┬───────┤┌──────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min    │avg    │max    ││stable││partition││adaptive │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│blitsort       ││n      │n log n│n log n││1      │1      │1      ││yes   ││yes      ││yes      │
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
|stablesort |   100000 |   32 | 0.006146 | 0.006190 |         1 |     100 |     random order |
|  blitsort |   100000 |   32 | 0.002231 | 0.002346 |         1 |     100 |     random order |
|   timsort |   100000 |   32 | 0.007395 | 0.007428 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003992 | 0.004017 |         1 |     100 |     random % 100 |
|  blitsort |   100000 |   32 | 0.001045 | 0.001137 |         1 |     100 |     random % 100 |
|   timsort |   100000 |   32 | 0.005343 | 0.005375 |         1 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000816 | 0.000835 |         1 |     100 |  ascending order |
|  blitsort |   100000 |   32 | 0.000045 | 0.000046 |         1 |     100 |  ascending order |
|   timsort |   100000 |   32 | 0.000044 | 0.000044 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001512 | 0.001535 |         1 |     100 |    ascending saw |
|  blitsort |   100000 |   32 | 0.000987 | 0.000997 |         1 |     100 |    ascending saw |
|   timsort |   100000 |   32 | 0.000833 | 0.000842 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000901 | 0.000925 |         1 |     100 |       pipe organ |
|  blitsort |   100000 |   32 | 0.000480 | 0.000487 |         1 |     100 |       pipe organ |
|   timsort |   100000 |   32 | 0.000175 | 0.000177 |         1 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.000932 | 0.000952 |         1 |     100 | descending order |
|  blitsort |   100000 |   32 | 0.000057 | 0.000057 |         1 |     100 | descending order |
|   timsort |   100000 |   32 | 0.000101 | 0.000102 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001512 | 0.001542 |         1 |     100 |   descending saw |
|  blitsort |   100000 |   32 | 0.000996 | 0.001004 |         1 |     100 |   descending saw |
|   timsort |   100000 |   32 | 0.000832 | 0.000843 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.002164 | 0.002193 |         1 |     100 |      random tail |
|  blitsort |   100000 |   32 | 0.001256 | 0.001263 |         1 |     100 |      random tail |
|   timsort |   100000 |   32 | 0.001947 | 0.001966 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.003645 | 0.003680 |         1 |     100 |      random half |
|  blitsort |   100000 |   32 | 0.002182 | 0.002199 |         1 |     100 |      random half |
|   timsort |   100000 |   32 | 0.003920 | 0.003937 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001128 | 0.001149 |         1 |     100 |  ascending tiles |
|  blitsort |   100000 |   32 | 0.001377 | 0.001388 |         1 |     100 |  ascending tiles |
|   timsort |   100000 |   32 | 0.000900 | 0.000977 |         1 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|stablesort |   100000 |   32 | 0.001778 | 0.001943 |         1 |     100 |     bit reversal |
|  blitsort |   100000 |   32 | 0.002019 | 0.002130 |         1 |     100 |     bit reversal |
|   timsort |   100000 |   32 | 0.002264 | 0.002612 |         1 |     100 |     bit reversal |

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
|stablesort |        8 |   32 | 0.006389 | 0.006428 |     65536 |     100 |         random 8 |
|  blitsort |        8 |   32 | 0.001636 | 0.001682 |     65536 |     100 |         random 8 |
|   timsort |        8 |   32 | 0.006283 | 0.006568 |     65536 |     100 |         random 8 |
|           |          |      |          |          |           |         |                  |
|stablesort |       32 |   32 | 0.009746 | 0.009944 |     16384 |     100 |        random 32 |
|  blitsort |       32 |   32 | 0.004181 | 0.004231 |     16384 |     100 |        random 32 |
|   timsort |       32 |   32 | 0.012812 | 0.013001 |     16384 |     100 |        random 32 |
|           |          |      |          |          |           |         |                  |
|stablesort |      128 |   32 | 0.013460 | 0.013539 |      4096 |     100 |       random 128 |
|  blitsort |      128 |   32 | 0.005662 | 0.005752 |      4096 |     100 |       random 128 |
|   timsort |      128 |   32 | 0.019040 | 0.019209 |      4096 |     100 |       random 128 |
|           |          |      |          |          |           |         |                  |
|stablesort |      512 |   32 | 0.017280 | 0.017404 |      1024 |     100 |       random 512 |
|  blitsort |      512 |   32 | 0.006601 | 0.006748 |      1024 |     100 |       random 512 |
|   timsort |      512 |   32 | 0.023626 | 0.023806 |      1024 |     100 |       random 512 |
|           |          |      |          |          |           |         |                  |
|stablesort |     2048 |   32 | 0.021079 | 0.021162 |       256 |     100 |      random 2048 |
|  blitsort |     2048 |   32 | 0.007615 | 0.007732 |       256 |     100 |      random 2048 |
|   timsort |     2048 |   32 | 0.027803 | 0.027929 |       256 |     100 |      random 2048 |
|           |          |      |          |          |           |         |                  |
|stablesort |     8192 |   32 | 0.024940 | 0.025083 |        64 |     100 |      random 8192 |
|  blitsort |     8192 |   32 | 0.008841 | 0.008983 |        64 |     100 |      random 8192 |
|   timsort |     8192 |   32 | 0.031903 | 0.032057 |        64 |     100 |      random 8192 |
|           |          |      |          |          |           |         |                  |
|stablesort |    32768 |   32 | 0.028798 | 0.028934 |        16 |     100 |     random 32768 |
|  blitsort |    32768 |   32 | 0.010548 | 0.010704 |        16 |     100 |     random 32768 |
|   timsort |    32768 |   32 | 0.036249 | 0.036422 |        16 |     100 |     random 32768 |
|           |          |      |          |          |           |         |                  |
|stablesort |   131072 |   32 | 0.032843 | 0.032996 |         4 |     100 |    random 131072 |
|  blitsort |   131072 |   32 | 0.012136 | 0.012574 |         4 |     100 |    random 131072 |
|   timsort |   131072 |   32 | 0.040239 | 0.040390 |         4 |     100 |    random 131072 |
|           |          |      |          |          |           |         |                  |
|stablesort |   524288 |   32 | 0.036935 | 0.037105 |         1 |     100 |    random 524288 |
|  blitsort |   524288 |   32 | 0.014131 | 0.015079 |         1 |     100 |    random 524288 |
|   timsort |   524288 |   32 | 0.044542 | 0.044729 |         1 |     100 |    random 524288 |

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
|     qsort |   100000 |   64 | 0.016954 | 0.017241 |   1536381 |     100 |    random string |
|  blitsort |   100000 |   64 | 0.010905 | 0.011249 |   1884961 |     100 |    random string |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |  128 | 0.018763 | 0.019204 |   1536491 |     100 |     random order |
|  blitsort |   100000 |  128 | 0.014102 | 0.014578 |   1884486 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.009507 | 0.009745 |   1536491 |     100 |     random order |
|  blitsort |   100000 |   64 | 0.004829 | 0.004964 |   1884486 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   32 | 0.008850 | 0.009087 |   1536634 |     100 |     random order |
|  blitsort |   100000 |   32 | 0.004073 | 0.004232 |   1892719 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006838 | 0.007074 |   1532465 |     100 |     random % 100 |
|  blitsort |   100000 |   32 | 0.001866 | 0.001955 |    972114 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002617 | 0.002843 |    815024 |     100 |  ascending order |
|  blitsort |   100000 |   32 | 0.000157 | 0.000163 |     99999 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003386 | 0.003545 |    915020 |     100 |    ascending saw |
|  blitsort |   100000 |   32 | 0.001429 | 0.001450 |    460578 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002680 | 0.002885 |    884462 |     100 |       pipe organ |
|  blitsort |   100000 |   32 | 0.000798 | 0.000814 |    358810 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002483 | 0.002640 |    853904 |     100 | descending order |
|  blitsort |   100000 |   32 | 0.000184 | 0.000185 |     99999 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003309 | 0.003457 |    953892 |     100 |   descending saw |
|  blitsort |   100000 |   32 | 0.001417 | 0.001441 |    472712 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004216 | 0.004406 |   1012073 |     100 |      random tail |
|  blitsort |   100000 |   32 | 0.001811 | 0.001844 |    657831 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005940 | 0.006148 |   1200713 |     100 |      random half |
|  blitsort |   100000 |   32 | 0.002999 | 0.003049 |   1074820 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003912 | 0.004184 |   1209200 |     100 |  ascending tiles |
|  blitsort |   100000 |   32 | 0.001386 | 0.001444 |    446579 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005230 | 0.006049 |   1553378 |     100 |     bit reversal |
|  blitsort |   100000 |   32 | 0.003875 | 0.004115 |   1895179 |     100 |     bit reversal |

</details>

Benchmark: blitsort vs pdqsort
------------------------------
The following benchmark was on WSL gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04) using the [wolfsort](https://github.com/scandum/wolfsort) benchmark.
The source code was compiled using g++ -O3 -w -fpermissive bench.c. The bar graph shows the best run out of 100 on 100,000 32 bit integers. Comparisons for blitsort and pdqsort are inlined.

![Graph](/images/graph5.png)

<details><summary><b>data table</b></summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   pdqsort |   100000 |   64 | 0.002658 | 0.002692 |         1 |     100 |     random order |
|  blitsort |   100000 |   64 | 0.002415 | 0.002508 |         1 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|   pdqsort |   100000 |   32 | 0.002683 | 0.002708 |         1 |     100 |     random order |
|  blitsort |   100000 |   32 | 0.002239 | 0.002334 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.000786 | 0.000793 |         1 |     100 |     random % 100 |
|  blitsort |   100000 |   32 | 0.001041 | 0.001105 |         1 |     100 |     random % 100 |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.000091 | 0.000092 |         1 |     100 |  ascending order |
|  blitsort |   100000 |   32 | 0.000043 | 0.000044 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.003460 | 0.003483 |         1 |     100 |    ascending saw |
|  blitsort |   100000 |   32 | 0.000749 | 0.000757 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002834 | 0.002862 |         1 |     100 |       pipe organ |
|  blitsort |   100000 |   32 | 0.000387 | 0.000390 |         1 |     100 |       pipe organ |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.000194 | 0.000197 |         1 |     100 | descending order |
|  blitsort |   100000 |   32 | 0.000055 | 0.000057 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.003233 | 0.003253 |         1 |     100 |   descending saw |
|  blitsort |   100000 |   32 | 0.000756 | 0.000768 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002239 | 0.002255 |         1 |     100 |      random tail |
|  blitsort |   100000 |   32 | 0.000409 | 0.000415 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002665 | 0.002685 |         1 |     100 |      random half |
|  blitsort |   100000 |   32 | 0.001369 | 0.001376 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002308 | 0.002331 |         1 |     100 |  ascending tiles |
|  blitsort |   100000 |   32 | 0.001374 | 0.001389 |         1 |     100 |  ascending tiles |
|           |          |      |          |          |           |         |                  |
|   pdqsort |   100000 |   32 | 0.002664 | 0.002685 |         1 |     100 |     bit reversal |
|  blitsort |   100000 |   32 | 0.002015 | 0.002120 |         1 |     100 |     bit reversal |

</details>
