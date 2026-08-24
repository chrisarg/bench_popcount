# Simple benchmark of various apporahces to compute the popcount using portable intrinsics (library SIMDe) or libpopcnt.

Compile with clang as: clang -O3 -march=native popcount_bench.c -o popcount_bench -lm

## Benchmarks under high level optimizations

We will test guarded and non guarded scalar versions of the code, AVX2 Harley Searle based on AVX2, AVX512 (2 varieties, one involving the _mm512_ternarylogic_epi64 _ternary_ logic, another that does not use said logic), AVX-512 population counts and the implementation in the library libpopcnt. The library SIMDe (SIMD everywhere) is used to emulate all vectorized instructions, even in processors that do not use them. The default pathway is to use -O3 without link time optimization (LTO). Passing the flag FLTO=-flto switches LTO

### i9-7900x

This is an AVX512 platform, without the VPOPCNTDQ 512 bit popcount instruction

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       8627.07 |       2196.10 |     1.00x
Scalar (Compiler -O3)     |       8630.38 |       1985.24 |     1.00x
AVX1 128-bit SIMDE        |       3727.37 |       1224.76 |     2.31x
AVX2                      |       2127.89 |        761.90 |     4.05x
AVX-512 (No Tern)         |       1518.11 |        690.84 |     5.68x
AVX-512 (Ternary)         |       1253.98 |        647.97 |     6.88x
AVX-512 (VPOPCNTDQ)       |       2326.67 |       1046.61 |     3.71x
NEON SIMDE                |       3609.23 |       1174.15 |     2.39x
LIBPOPCNT                 |       2040.85 |        910.60 |     4.23x
```


```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      28588.58 |       3048.90 |     1.00x
Scalar (Compiler -O3)     |       3511.25 |        958.60 |     8.14x
AVX1 128-bit SIMDE        |       3982.67 |       1074.71 |     7.18x
AVX2                      |       2362.90 |        665.11 |    12.10x
AVX-512 (No Tern)         |       1482.43 |        624.33 |    19.28x
AVX-512 (Ternary)         |        976.92 |        431.62 |    29.26x
AVX-512 (VPOPCNTDQ)       |       2143.89 |        706.20 |    13.33x
NEON SIMDE                |       4148.27 |        935.82 |     6.89x
LIBPOPCNT                 |       2266.74 |        866.08 |    12.61x
```

```bash
make -s clean && make CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       9332.12 |       1605.86 |     1.00x
Scalar (Compiler -O3)     |       3967.16 |       1035.81 |     2.35x
AVX1 128-bit SIMDE        |       4026.51 |       1028.59 |     2.32x
AVX2                      |       2407.10 |        964.75 |     3.88x
AVX-512 (No Tern)         |       1470.47 |        505.52 |     6.35x
AVX-512 (Ternary)         |       1007.02 |        525.54 |     9.27x
AVX-512 (VPOPCNTDQ)       |       2169.98 |        875.06 |     4.30x
NEON SIMDE                |       3930.46 |       1042.07 |     2.37x
LIBPOPCNT                 |       2263.82 |        744.64 |     4.12x
```

Now with LTO

```bash
make -s clean && make OPT_LEVEL=-O3 FLTO=-flto CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6674.49 |       1701.81 |     1.00x
Scalar (Compiler -O3)     |       6746.97 |       1772.45 |     0.99x
AVX1 128-bit SIMDE        |       3750.31 |       1174.58 |     1.78x
AVX2                      |       2161.43 |        851.52 |     3.09x
AVX-512 (No Tern)         |       1532.43 |        893.64 |     4.36x
AVX-512 (Ternary)         |       1264.94 |        795.41 |     5.28x
AVX-512 (VPOPCNTDQ)       |       2344.36 |        985.98 |     2.85x
NEON SIMDE                |       3645.48 |       1219.69 |     1.83x
LIBPOPCNT                 |       2055.32 |        900.02 |     3.25x
```

```bash
make -s clean && make OPT_LEVEL=-O3 FLTO=-flto CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      29896.87 |       7554.54 |     1.00x
Scalar (Compiler -O3)     |       3846.22 |       1582.02 |     7.77x
AVX1 128-bit SIMDE        |       4306.54 |       1883.55 |     6.94x
AVX2                      |       2555.05 |       1239.44 |    11.70x
AVX-512 (No Tern)         |       1589.89 |       1156.57 |    18.80x
AVX-512 (Ternary)         |       1094.02 |        735.21 |    27.33x
AVX-512 (VPOPCNTDQ)       |       2302.73 |       1205.44 |    12.98x
NEON SIMDE                |       4437.92 |       1746.77 |     6.74x
LIBPOPCNT                 |       2430.29 |       1131.64 |    12.30x
```

```bash
make -s clean && make OPT_LEVEL=-O3 FLTO=-flto CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       9545.11 |       2066.33 |     1.00x
Scalar (Compiler -O3)     |       2323.31 |        949.00 |     4.11x
AVX1 128-bit SIMDE        |       4106.36 |       1146.50 |     2.32x
AVX2                      |       2410.16 |        801.68 |     3.96x
AVX-512 (No Tern)         |       1496.87 |        740.37 |     6.38x
AVX-512 (Ternary)         |       1028.37 |        497.40 |     9.28x
AVX-512 (VPOPCNTDQ)       |       2213.37 |        903.78 |     4.31x
NEON SIMDE                |       3991.52 |       1212.43 |     2.39x
LIBPOPCNT                 |       2291.73 |        829.93 |     4.17x
```

### i7-11700

This is an AVX512 platform with hardware level VPOPCNTDQ.

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6203.07 |       1168.92 |     1.00x
Scalar (Compiler -O3)     |       1496.93 |        686.12 |     4.14x
AVX1 128-bit SIMDE        |       2848.97 |        603.54 |     2.18x
AVX2                      |       1592.88 |        435.87 |     3.89x
AVX-512 (No Tern)         |       1131.89 |        580.77 |     5.48x
AVX-512 (Ternary)         |        811.10 |        371.81 |     7.65x
AVX-512 (VPOPCNTDQ)       |        738.04 |        249.13 |     8.40x
NEON SIMDE                |       2899.56 |        795.05 |     2.14x
LIBPOPCNT                 |        568.19 |        315.61 |    10.92x
```


```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       5614.70 |       1142.58 |     1.00x
Scalar (Compiler -O3)     |        874.92 |         52.15 |     6.42x
AVX1 128-bit SIMDE        |       3041.19 |        343.77 |     1.85x
AVX2                      |       1608.01 |        388.76 |     3.49x
AVX-512 (No Tern)         |       1070.74 |        263.87 |     5.24x
AVX-512 (Ternary)         |        621.66 |        189.22 |     9.03x
AVX-512 (VPOPCNTDQ)       |        572.81 |         36.24 |     9.80x
NEON SIMDE                |       3048.08 |        323.18 |     1.84x
LIBPOPCNT                 |        561.13 |         24.24 |    10.01x
```


```bash
make -s clean && make CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6716.39 |        574.19 |     1.00x
Scalar (Compiler -O3)     |       1370.54 |        219.78 |     4.90x
AVX1 128-bit SIMDE        |       3037.45 |        417.81 |     2.21x
AVX2                      |       1496.25 |        351.39 |     4.49x
AVX-512 (No Tern)         |       1071.88 |        319.41 |     6.27x
AVX-512 (Ternary)         |        620.76 |        136.97 |    10.82x
AVX-512 (VPOPCNTDQ)       |        574.94 |         24.58 |    11.68x
NEON SIMDE                |       3055.23 |        320.84 |     2.20x
LIBPOPCNT                 |        609.95 |        224.37 |    11.01x
```

Now with FLTO

```bash
make -s clean && make OPT_LEVEL=-O3 FLTO=-flto CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6306.10 |       1447.73 |     1.00x
Scalar (Compiler -O3)     |       1997.94 |        788.97 |     3.16x
AVX1 128-bit SIMDE        |       2944.08 |        936.17 |     2.14x
AVX2                      |       1655.29 |        730.08 |     3.81x
AVX-512 (No Tern)         |       1158.16 |        468.16 |     5.44x
AVX-512 (Ternary)         |        845.97 |        496.05 |     7.45x
AVX-512 (VPOPCNTDQ)       |        761.11 |        445.98 |     8.29x
NEON SIMDE                |       2966.64 |       1033.45 |     2.13x
LIBPOPCNT                 |        583.41 |        158.97 |    10.81x
```

```bash
make -s clean && make OPT_LEVEL=-O3 FLTO=-flto CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       5782.47 |       1809.53 |     1.00x
Scalar (Compiler -O3)     |        908.25 |        410.31 |     6.37x
AVX1 128-bit SIMDE        |       3187.21 |       1236.51 |     1.81x
AVX2                      |       1687.92 |        836.40 |     3.43x
AVX-512 (No Tern)         |       1128.32 |        680.06 |     5.12x
AVX-512 (Ternary)         |        650.15 |        388.93 |     8.89x
AVX-512 (VPOPCNTDQ)       |        604.74 |        452.66 |     9.56x
NEON SIMDE                |       3173.35 |       1071.33 |     1.82x
LIBPOPCNT                 |        614.88 |        605.50 |     9.40x
```

```bash
make -s clean && make OPT_LEVEL=-O3 FLTO=-flto CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6973.66 |       1312.64 |     1.00x
Scalar (Compiler -O3)     |        765.60 |        351.99 |     9.11x
AVX1 128-bit SIMDE        |       3198.68 |        988.10 |     2.18x
AVX2                      |       1576.39 |        647.70 |     4.42x
AVX-512 (No Tern)         |       1127.25 |        561.85 |     6.19x
AVX-512 (Ternary)         |        652.94 |        389.54 |    10.68x
AVX-512 (VPOPCNTDQ)       |        605.82 |        371.18 |    11.51x
NEON SIMDE                |       3183.65 |        896.08 |     2.19x
LIBPOPCNT                 |        663.06 |        585.55 |    10.52x
```

### Intel Xeon 6517P

This is an AVX512 platform with hardware level VPOPCNTQ

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       5873.50 |       2447.24 |     1.00x
Scalar (Compiler -O3)     |       2062.25 |        855.24 |     2.85x
AVX1 128-bit SIMDE        |       5234.70 |       2192.19 |     1.12x
AVX2                      |       2863.83 |       1178.13 |     2.05x
AVX-512 (No Tern)         |       2232.06 |        900.65 |     2.63x
AVX-512 (Ternary)         |       1206.96 |        459.08 |     4.87x
AVX-512 (VPOPCNTDQ)       |       1272.16 |        442.50 |     4.62x
NEON SIMDE                |       5282.13 |       2206.38 |     1.11x
LIBPOPCNT                 |       1138.65 |        427.46 |     5.16x
```


```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6999.94 |       2171.29 |     1.00x
Scalar (Compiler -O3)     |       1278.14 |        395.36 |     5.48x
AVX1 128-bit SIMDE        |       4564.32 |       1456.11 |     1.53x
AVX2                      |       2389.46 |        768.84 |     2.93x
AVX-512 (No Tern)         |       1771.87 |        596.06 |     3.95x
AVX-512 (Ternary)         |        977.44 |        282.44 |     7.16x
AVX-512 (VPOPCNTDQ)       |        979.88 |        232.23 |     7.14x
NEON SIMDE                |       4542.36 |       1445.96 |     1.54x
LIBPOPCNT                 |       1012.90 |        278.26 |     6.91x
```


```bash
make -s clean && make CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       8171.39 |       3575.41 |     1.00x
Scalar (Compiler -O3)     |       1533.70 |        522.84 |     5.33x
AVX1 128-bit SIMDE        |       4837.40 |       2093.72 |     1.69x
AVX2                      |       2569.72 |       1106.08 |     3.18x
AVX-512 (No Tern)         |       1733.52 |        802.04 |     4.71x
AVX-512 (Ternary)         |       1072.39 |        385.71 |     7.62x
AVX-512 (VPOPCNTDQ)       |       1043.07 |        340.49 |     7.83x
NEON SIMDE                |       4687.68 |       2007.62 |     1.74x
LIBPOPCNT                 |       1029.76 |        364.34 |     7.94x
```

### AMD EPYC 7352

This is an AVX2 platform without hardware level VPOPCNTDQ.

Benchmarks for GCC, CLANG

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      10460.76 |        477.72 |     1.00x
Scalar (Compiler -O3)     |      10458.47 |        406.47 |     1.00x
AVX1 128-bit SIMDE        |       4419.78 |        758.97 |     2.37x
AVX2                      |       2344.06 |        138.98 |     4.46x
AVX-512 (No Tern)         |       5554.61 |        289.16 |     1.88x
AVX-512 (Ternary)         |      23811.50 |        764.52 |     0.44x
AVX-512 (VPOPCNTDQ)       |      20247.34 |        666.75 |     0.52x
NEON SIMDE                |       4532.37 |        337.50 |     2.31x
LIBPOPCNT                 |       2279.09 |        174.32 |     4.59x
```


```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      11049.64 |        732.84 |     1.00x
Scalar (Compiler -O3)     |       3262.96 |        315.29 |     3.39x
AVX1 128-bit SIMDE        |       4359.10 |        464.75 |     2.53x
AVX2                      |       2618.49 |        235.89 |     4.22x
AVX-512 (No Tern)         |       2504.62 |        273.52 |     4.41x
AVX-512 (Ternary)         |      11051.51 |       1117.51 |     1.00x
AVX-512 (VPOPCNTDQ)       |       3293.40 |        300.84 |     3.36x
NEON SIMDE                |       4332.65 |        489.83 |     2.55x
LIBPOPCNT                 |       2370.21 |        213.08 |     4.66x
```

### i3-12100t

This is an AVX2 platform without hardware level VPOPCNTDQ.

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       4080.56 |        253.17 |     1.00x
Scalar (Compiler -O3)     |       4086.17 |        333.17 |     1.00x
AVX1 128-bit SIMDE        |       3866.79 |        123.99 |     1.06x
AVX2                      |       2163.19 |         74.94 |     1.89x
AVX-512 (No Tern)         |       2526.06 |         85.42 |     1.62x
AVX-512 (Ternary)         |       7688.51 |        242.61 |     0.53x
AVX-512 (VPOPCNTDQ)       |       3903.79 |        128.07 |     1.05x
NEON SIMDE                |       3871.81 |        110.00 |     1.05x
LIBPOPCNT                 |       2107.04 |         79.83 |     1.94x
```

```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6450.18 |        705.85 |     1.00x
Scalar (Compiler -O3)     |       2758.52 |        106.49 |     2.34x
AVX1 128-bit SIMDE        |       3977.03 |        146.42 |     1.62x
AVX2                      |       2280.00 |        103.84 |     2.83x
AVX-512 (No Tern)         |       1999.02 |         84.46 |     3.23x
AVX-512 (Ternary)         |       3762.68 |        141.15 |     1.71x
AVX-512 (VPOPCNTDQ)       |       2767.02 |        101.21 |     2.33x
NEON SIMDE                |       3985.53 |        149.42 |     1.62x
LIBPOPCNT                 |       2024.61 |         80.77 |     3.19x
```

```bash
make -s clean && make CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6349.48 |        309.72 |     1.00x
Scalar (Compiler -O3)     |       2439.26 |        122.31 |     2.60x
AVX1 128-bit SIMDE        |       4010.76 |        187.02 |     1.58x
AVX2                      |       2327.07 |        118.03 |     2.73x
AVX-512 (No Tern)         |       1995.83 |        111.29 |     3.18x
AVX-512 (Ternary)         |       1900.65 |        127.19 |     3.34x
AVX-512 (VPOPCNTDQ)       |       2788.81 |        166.77 |     2.28x
NEON SIMDE                |       4017.92 |        200.78 |     1.58x
LIBPOPCNT                 |       2038.88 |        101.65 |     3.11x
```

### Xeon E-2697v4

This is an AVX2 platform without hardware level VPOPCNTDQ.

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      10928.28 |        390.54 |     1.00x
Scalar (Compiler -O3)     |      11067.52 |        446.25 |     0.99x
AVX1 128-bit SIMDE        |       7777.88 |        640.83 |     1.41x
AVX2                      |       5394.92 |        350.48 |     2.03x
AVX-512 (No Tern)         |       8545.93 |        375.40 |     1.28x
AVX-512 (Ternary)         |      32895.47 |        787.97 |     0.33x
AVX-512 (VPOPCNTDQ)       |      24616.20 |        571.84 |     0.44x
NEON SIMDE                |       8436.73 |        386.02 |     1.30x
LIBPOPCNT                 |       4904.84 |        290.69 |     2.23x
```


```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      48323.49 |       1072.39 |     1.00x
Scalar (Compiler -O3)     |       6820.53 |        469.46 |     7.09x
AVX1 128-bit SIMDE        |       8895.75 |        384.65 |     5.43x
AVX2                      |       5416.53 |        276.99 |     8.92x
AVX-512 (No Tern)         |       4856.93 |        307.41 |     9.95x
AVX-512 (Ternary)         |       9449.05 |        357.43 |     5.11x
AVX-512 (VPOPCNTDQ)       |       6845.82 |        320.30 |     7.06x
NEON SIMDE                |      17086.68 |        474.77 |     2.83x
LIBPOPCNT                 |       5323.83 |        301.04 |     9.08x
```


```bash
make -s clean && make CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      16697.52 |        468.67 |     1.00x
Scalar (Compiler -O3)     |       7605.25 |        229.67 |     2.20x
AVX1 128-bit SIMDE        |       8876.71 |        450.03 |     1.88x
AVX2                      |       5279.51 |        271.12 |     3.16x
AVX-512 (No Tern)         |       4929.61 |        221.73 |     3.39x
AVX-512 (Ternary)         |       8989.63 |        307.57 |     1.86x
AVX-512 (VPOPCNTDQ)       |       6922.51 |        266.53 |     2.41x
NEON SIMDE                |       9040.08 |        621.60 |     1.85x
LIBPOPCNT                 |       5352.74 |        290.37 |     3.12x
```

### rockpro64 (Rockchip RK3399, 2 x A-72 and 2 x A 53)

This is a Neon platform with hardware level vcntq_u8.

Benchmarks for GCC, CLANG

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      37325.69 |       1087.46 |     1.00x
Scalar (Compiler -O3)     |      37307.61 |        951.41 |     1.00x
AVX1 128-bit SIMDE        |      26103.19 |        759.67 |     1.43x
AVX2                      |      71813.82 |       1255.75 |     0.52x
AVX-512 (No Tern)         |      31004.24 |        948.43 |     1.20x
AVX-512 (Ternary)         |      53320.02 |       1171.84 |     0.70x
AVX-512 (VPOPCNTDQ)       |      55945.90 |       1813.92 |     0.67x
NEON SIMDE                |      25784.07 |       1061.34 |     1.45x
LIBPOPCNT                 |      14496.90 |        770.91 |     2.57x
```

```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      71856.40 |       1250.45 |     1.00x
Scalar (Compiler -O3)     |      25352.76 |        922.11 |     2.83x
AVX1 128-bit SIMDE        |      28116.19 |        890.63 |     2.56x
AVX2                      |      23692.86 |       2035.82 |     3.03x
AVX-512 (No Tern)         |      30690.93 |       1273.09 |     2.34x
AVX-512 (Ternary)         |      40796.98 |       2169.44 |     1.76x
AVX-512 (VPOPCNTDQ)       |      21496.18 |       2552.69 |     3.34x
NEON SIMDE                |      28088.02 |        589.34 |     2.56x
LIBPOPCNT                 |      14747.00 |       1060.17 |     4.87x
```

### rock64 (Rockchip RK3328 4 x A53)

This is a Neon platform with hardware level vcntq_u8.

Benchmarks for GCC, CLANG

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |     140895.75 |       5748.48 |     1.00x
Scalar (Compiler -O3)     |     128181.19 |       4318.88 |     1.10x
AVX1 128-bit SIMDE        |      50639.87 |       3614.43 |     2.78x
AVX2                      |     174004.88 |       6148.64 |     0.81x
AVX-512 (No Tern)         |      81791.03 |       5245.01 |     1.72x
AVX-512 (Ternary)         |     119870.69 |       5633.66 |     1.18x
AVX-512 (VPOPCNTDQ)       |      93811.77 |       4209.44 |     1.50x
NEON SIMDE                |      49766.75 |       3103.19 |     2.83x
LIBPOPCNT                 |      34350.46 |       2820.81 |     4.10x
```



```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |     141233.52 |       8525.39 |     1.00x
Scalar (Compiler -O3)     |      55865.64 |       5919.28 |     2.53x
AVX1 128-bit SIMDE        |      48654.78 |       4433.61 |     2.90x
AVX2                      |      50290.07 |       5116.07 |     2.81x
AVX-512 (No Tern)         |      72631.42 |       6905.62 |     1.94x
AVX-512 (Ternary)         |     100352.25 |       7580.36 |     1.41x
AVX-512 (VPOPCNTDQ)       |      48526.30 |       5296.73 |     2.91x
NEON SIMDE                |      48729.82 |       5349.06 |     2.90x
LIBPOPCNT                 |      34644.75 |       4234.52 |     4.08x
```

## Benchmarks that turn off optimization for all code paths

We will test guarded and non guarded scalar versions of the code, AVX2 Harley Searle based on AVX2, AVX512 (2 varieties, one involving the _mm512_ternarylogic_epi64 _ternary_ logic, another that does not use said logic), AVX-512 population counts and the implementation in the library libpopcnt. The library SIMDe (SIMD everywhere) is used to emulate all vectorized instructions, even in processors that do not use them. We will explicitly turn off optimization at the translation unit level by turning off optimization using the OPT_LABEL flag

### i9-7900x (-O0)

This is an AVX512 platform, without the VPOPCNTDQ 512 bit popcount instruction

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make OPT_LEVEL=-O0  CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      35074.31 |       4363.02 |     1.00x
Scalar (Compiler -O0)     |      33849.22 |       4154.23 |     1.04x
AVX1 128-bit SIMDE        |      97160.98 |      10667.26 |     0.36x
AVX2                      |      54560.45 |       6848.55 |     0.64x
AVX-512 (No Tern)         |      30824.01 |       6942.21 |     1.14x
AVX-512 (Ternary)         |      13448.23 |       2190.71 |     2.61x
AVX-512 (VPOPCNTDQ)       |     215259.67 |      20259.89 |     0.16x
NEON SIMDE                |     692959.35 |      48440.47 |     0.05x
LIBPOPCNT                 |      57983.72 |       6771.46 |     0.60x
```


```bash
make -s clean && make OPT_LEVEL=-O0  CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      35388.23 |       4886.08 |     1.00x
Scalar (Compiler -O0)     |      27429.29 |       3987.56 |     1.29x
AVX1 128-bit SIMDE        |      92711.03 |      12611.34 |     0.38x
AVX2                      |      58134.73 |       9797.36 |     0.61x
AVX-512 (No Tern)         |      46062.92 |      13681.26 |     0.77x
AVX-512 (Ternary)         |      13536.29 |       2200.99 |     2.61x
AVX-512 (VPOPCNTDQ)       |     212557.31 |      21487.79 |     0.17x
NEON SIMDE                |     815400.40 |      61157.72 |     0.04x
LIBPOPCNT                 |      63576.37 |      11357.64 |     0.56x
```


```bash
make -s clean && make OPT_LEVEL=-O0  CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      35245.59 |       5608.85 |     1.00x
Scalar (Compiler -O0)     |      27403.08 |       4523.55 |     1.29x
AVX1 128-bit SIMDE        |      97744.01 |      12212.79 |     0.36x
AVX2                      |      58627.62 |       9134.79 |     0.60x
AVX-512 (No Tern)         |      47966.37 |      12839.45 |     0.73x
AVX-512 (Ternary)         |      13011.49 |       2109.92 |     2.71x
AVX-512 (VPOPCNTDQ)       |     212097.22 |      17168.22 |     0.17x
NEON SIMDE                |     814135.89 |      48116.49 |     0.04x
LIBPOPCNT                 |      63103.16 |      10580.17 |     0.56x
```

### i7-11700 (-O0)

This is an AVX512 platform, sith the VPOPCNTDQ 512 bit popcount instruction

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make OPT_LEVEL=-O0 CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      20645.25 |       2026.57 |     1.00x
Scalar (Compiler -O0)     |      22930.86 |       2930.88 |     0.90x
AVX1 128-bit SIMDE        |      81516.34 |       6041.48 |     0.25x
AVX2                      |      43930.13 |       3105.25 |     0.47x
AVX-512 (No Tern)         |      23926.97 |       8791.50 |     0.86x
AVX-512 (Ternary)         |      10283.28 |       1659.50 |     2.01x
AVX-512 (VPOPCNTDQ)       |       9189.48 |       1450.32 |     2.25x
NEON SIMDE                |     560131.36 |      22798.07 |     0.04x
LIBPOPCNT                 |       6162.60 |       1064.50 |     3.35x
```

```bash
make -s clean && make OPT_LEVEL=-O0 CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      15327.19 |       2728.74 |     1.00x
Scalar (Compiler -O0)     |      15135.48 |       3973.16 |     1.01x
AVX1 128-bit SIMDE        |      75934.44 |       5947.60 |     0.20x
AVX2                      |      41133.25 |       3085.15 |     0.37x
AVX-512 (No Tern)         |      24097.16 |       2273.31 |     0.64x
AVX-512 (Ternary)         |       9046.02 |       1272.63 |     1.69x
AVX-512 (VPOPCNTDQ)       |       9034.51 |       1073.17 |     1.70x
NEON SIMDE                |     711015.41 |      23378.60 |     0.02x
LIBPOPCNT                 |      10257.86 |       1089.02 |     1.49x
```

```bash
make -s clean && make OPT_LEVEL=-O0 CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      15471.13 |       3107.75 |     1.00x
Scalar (Compiler -O0)     |      14724.62 |       4022.06 |     1.05x
AVX1 128-bit SIMDE        |      75562.66 |       5663.81 |     0.20x
AVX2                      |      41524.22 |       3506.27 |     0.37x
AVX-512 (No Tern)         |      24654.79 |       2189.96 |     0.63x
AVX-512 (Ternary)         |      10068.03 |       1177.52 |     1.54x
AVX-512 (VPOPCNTDQ)       |       9057.23 |       1140.12 |     1.71x
NEON SIMDE                |     711561.23 |      37631.36 |     0.02x
LIBPOPCNT                 |      10269.37 |       1167.24 |     1.51x
```
