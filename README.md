# SimpleProfiler - A basic macro-based header-only C++ profiler

# What does it do?

`SimpleProfiler` measures the run time of code segments similar to tic/toc meachnisms in other languages. It comes with the following features:

- Single header file `SimpleProfiler.h`
- Multiple timers per profiler instance
- Multiple profiler instances (`SimpleProfiler`) in the same file
- Averages over multiple runs of the same code segments
- Time measurements are reported to terminal or a copy of source code file with annotations

# How to use?

1. Place `profiler.h` in search path of C++ compiler (e.g. same directory as the source file that should be profiled).
2. Copy the following profiler block to the top of the source file that should be profiled
```
#define PROFILER_ON             // comment out to ignore all profiler macros
//#define RCOUT_ON              // use Rcpp::Rcout instead of std::cout
char THIS_FILE[] = __FILE__ ;   // use current file, this works only if binary is called from same directory as source
// char THIS_FILE[] = "/home/user/file_to_be_profiled.cpp"; // alternatively, use absolute path to source file
#include "SimpleProfiler.h"
```
3. Place time measurment points (`TIC`) inside the source file. Time is measured between `TIC` time stamps.

Overview of commands and pre-defined macros:

| Commands and pre-defined macros:                  | Commands                               | Macro short cuts   |
|---------------------------------------------------|----------------------------------------|--------------------|
| Initialize profiler instance (outside of main()): | SimpleProfiler myprofiler1(THIS_FILE); | TIC_INIT           |
| Set time stamp:                                   | myprofiler1.tic(__LINE__);             | TIC                |
| Compute results (in terminal only):               | myprofiler1.output(false, true, 2);    | TIC_PRINT          |
| Compute results (annotation only):                | myprofiler1.output(true, false, 2);    | TIC_ANNOTATE       |
| Compute results (do both):                        | myprofiler1.output(true, true, 2);     | TIC_ANNOTATE_PRINT |

Arguments for `.output( , , TIC_TYPE)`: `TIC_TYPE = 0..` nanoseconds, `1..` microseconds, `2..` milliseconds (default), `3..` seconds

# What to look out for?

- Do not use inside multiple threads.
- Do not use several TIC time stamps in one line.
- Be careful with optimization flags: SimpleProfiler might hamper aggressive optimization for trivial code segements. Use to measure actual work.
