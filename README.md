# Simple benchmark of various apporahces to compute the popcount using portable intrinsics (library SIMDe) or libpopcnt.
Compile with clang as: clang -O3 -march=native popcount_bench.c -o popcount_bench -lm

## Results

We will test guarded and non guarded scalar versions of the code, AVX2 Harley Searle based on AVX2, AVX512 (2 varieties, one involving the _mm512_ternarylogic_epi64 _ternary_ logic, another that does not use said logic), AVX-512 population counts and the implementation in the library libpopcnt. The library SIMDe (SIMD everywhere) is used to emulate all vectorized instructions, even in processors that do not use them.

# i9-7900x

This is an AVX512 platform, without the VPOPCNTDQ 512 bit popcount instruction

Benchmarks for GCC, CLANG, ICX

```bash
ake -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
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

## i7-11700

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


## i3-12100t

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

## Xeon E-2697v4

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

## rockpro64 (Rockchip RK3399, 2 x A-72 and 2 x A 53)

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

## rock64 (Rockchip RK3328 4 x A53)

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