// --- test_profiler.cpp ---
// A Test source file to test macro-based profiler
// - creates two profiler instances: myprofiler1 and myprofiler2
// - myprofiler1 is used via macros, myprofiler2 is used directly

// build: g++ -Wall test_profiler.cpp -o test_profiler

#include <iostream>
#include <string>
#include <time.h> // for sleep() 
#include <math.h>

// --- PROFILER BLOCK -------------------------------------------------
#define PROFILER_ON      // comment out to ignore all profiler macros
//#define RCOUT_ON       // use Rcout instead of cout
char THIS_FILE[] = __FILE__ ;
#include "SimpleProfiler.h"
//---------------------------------------------------------------------

void sleep(unsigned int mseconds) {
	clock_t goal = mseconds + clock();
	while (goal > clock());
}

TIC_INIT // short for: SimpleProfiler myprofiler1(THIS_FILE);

SimpleProfiler myprofiler2(THIS_FILE);

int main() {

    for (int i = 0; i < 5; i++) {
    
    TIC // short for: myprofiler1.tic(__LINE__);
    myprofiler2.tic(__LINE__);
    
    sleep(1000000);

    TIC // short for: myprofiler1.tic(__LINE__);
    myprofiler2.tic(__LINE__);
    
    }
    
    myprofiler1.showInfo();
    myprofiler1.showData();
    
    TIC_ANNOTATE_PRINT // short for: myprofiler1.output();
    myprofiler2.output(true, true, 3);
    
	return 0;
}
