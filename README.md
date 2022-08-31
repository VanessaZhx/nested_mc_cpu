# CPU Implementation of *GPUs for Risk Management*

*Vanessa Zhu*

*Computing (Management and Finance), Imperial College London*

----

This is the CPU baseline implementation of MSc Individual Project *GPUs for Risk Management* .
- For the GPU implementation, see <https://github.com/VanessaZhx/nested_mc_gpu>
- For the Moro's ICDF implementation test, see <https://github.com/VanessaZhx/MoroInverseTest>

## Install
This project uses [CUDA Toolkit](https://developer.nvidia.com/cuda-toolkit) and [QuantLib](https://www.quantlib.org/download.shtml). Please ensure they are locally installed and change the directory in `CMakeLists.txt` accordingly.

Clone the repository.
```console
$ git clone https://github.com/VanessaZhx/nested_mc_cpu.git
```
Build a folder for compiling.
```console
$ mkdir ./build
$ cd build
```

Compile the code.
```console
$ cmake ..
$ cmake --build .
```

Run the code by file `output`.
```console
$ ./output 1024 1024 10
```

## Usage
```console
$ ./output -h

===================== USAGE =====================
Enter up to 3 numbers for [path_ext, path_int, exp_times]
Default setup: [10, 10, 0]

```
Sample output:
```console
$ ./output 10 10 100

== SET UP ==
Experiment Times: 100
Path External: 10
Path Internal: 10

== DEVICE ==
Linux version 5.15.0-41-generic (buildd@lcy02-amd64-105) (gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0, GNU ld (GNU Binutils for Ubuntu) 2.34) #44~20.04.1-Ubuntu SMP Fri Jun 24 13:27:29 UTC 2022
processor	: 0
vendor_id	: GenuineIntel
cpu family	: 6
model		: 63
model name	: Intel(R) Xeon(R) CPU E5-1630 v3 @ 3.70GHz
stepping	: 2
microcode	: 0x49
cpu MHz		: 3791.434
cache size	: 10240 KB
physical id	: 0
siblings	: 8
core id		: 0
cpu cores	: 4
apicid		: 0
initial apicid	: 0
fpu		: yes
fpu_exception	: yes
cpuid level	: 15
wp		: yes
flags		: fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm cpuid_fault epb invpcid_single pti intel_ppin ssbd ibrs ibpb stibp tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid cqm xsaveopt cqm_llc cqm_occup_llc dtherm ida arat pln pts md_clear flush_l1d
vmx flags	: vnmi preemption_timer posted_intr invvpid ept_x_only ept_ad ept_1gb flexpriority apicv tsc_offset vtpr mtf vapic ept vpid unrestricted_guest vapic_reg vid ple shadow_vmcs
bugs		: cpu_meltdown spectre_v1 spectre_v2 spec_store_bypass l1tf mds swapgs itlb_multihit mmio_stale_data
bogomips	: 7383.32
clflush size	: 64
cache_alignment	: 64
address sizes	: 46 bits physical, 48 bits virtual
power management:



== EXECUTION ==
Experiment # 0 finished.
Experiment # 10 finished.
Experiment # 20 finished.
Experiment # 30 finished.
Experiment # 40 finished.
Experiment # 50 finished.
Experiment # 60 finished.
Experiment # 70 finished.
Experiment # 80 finished.
Experiment # 90 finished.
Experiment # 99 finished.
AVG EXECUTION TIME: 0.234853 ms.
```


## Structure
`main.cpp` - The main entrance to the model. Contains all the initialising parameters.

`NestedMonteCarloVaR.cpp` - The main implementation of MC. 

`RNG.cu` - Random number generator. 

`Stock.h` `Bond.h` `BasketOption.h` `BarrierOption.h` - Products classes. 

`cuda_helper.cuh` - Helper functions.
