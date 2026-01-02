/* SimpleProfiler v0.3
   A basic macro-based header-only profiler for C++
   by Philip Schuster, May, 2020 (last revision Jan, 2026)

Usage:	copy the following block to the source file to be profiled:

// --- PROFILER BLOCK -------------------------------------------------
#define PROFILER_ON             // comment out to ignore all profiler macros
//#define RCOUT_ON              // use Rcpp::Rcout instead of std::cout
char THIS_FILE[] = __FILE__ ;   // use current file, this works only if binary is called from same directory as source
// char THIS_FILE[] = "/home/user/file_to_be_profiled.cpp"; // use absolute path to source file
#include "SimpleProfiler.h"
//---------------------------------------------------------------------

Commands and pre-defined macros:                            Commands                                    Macro short cuts
    - Initialize profiler instance (outside of main()):     SimpleProfiler myprofiler1(THIS_FILE);      TIC_INIT
    - Set time stamp:                                       myprofiler1.tic(__LINE__);                  TIC
    - Compute results (in terminal only):                   myprofiler1.output(false, true, 2);         TIC_PRINT
    - Compute results (annotation only):                    myprofiler1.output(true, false, 2);         TIC_ANNOTATE
    - Compute results (do both):                            myprofiler1.output(true, true, 2);          TIC_ANNOTATE_PRINT
    
    Arguments: .output( , , TIC_TYPE): TIC_TYPE = 0.. nanoseconds, 1.. microseconds, 2.. milliseconds (default), 3.. seconds
    
Hints: Do not use several TIC; in one line. Careful with multiple threads.

*/


#include <iostream>
#include <string>
#include <fstream>  // for file acccess
#include <vector>
#include <iomanip>  // for std::setprecision(), std::setfill()
#include <chrono>   // for timer

#ifdef RCOUT_ON
    #define COUT Rcpp::Rcout
#else
    #define COUT std::cout
#endif
#ifdef PROFILER_ON

    class SimpleProfiler {
        
    private:
            std::string filename;
            int profilerid;
            int numlines;
            std::vector<long long> average_time;
            std::vector<long> call_counter;
            std::chrono::high_resolution_clock::time_point previous_time;
                
    public:
            static int ObjCounter;  // like a global counter
        
            SimpleProfiler(std::string in_filename) {
                profilerid = ++ObjCounter;
                filename = in_filename;
                numlines = readNumlines(in_filename);
                average_time.resize(numlines);
                call_counter.resize(numlines);
                previous_time = std::chrono::high_resolution_clock::now();
                //std::cout << "Object constructed! " << std::endl;
            }
            
            SimpleProfiler(std::string in_filename, int in_numlines) {
                profilerid = ++ObjCounter;
                filename = in_filename;
                numlines = in_numlines;
                average_time.resize(numlines);
                call_counter.resize(numlines);
                previous_time = std::chrono::high_resolution_clock::now();
            }
            
            ~SimpleProfiler() {
            }
            
            std::string getFilename() {
                return filename;
            }
            
            int getNumlines() {
                return numlines;
            }
            
            int getSizeAverageTime() {
                return average_time.size();
            }
            
            int getSizeCallCounter() {
                return call_counter.size();
            }
            
            void showData() {
                COUT << "Data entries: " << std::endl;
                for (int i = 1; i <= numlines; ++i) {
                    COUT << "Line " << std::fixed << std::setw( 4 ) << std::setprecision( 0 ) << std::setfill( ' ' ) << i << " | ";
                    COUT << "Calls " << std::fixed << std::setw( 10 ) << std::setprecision( 0 ) << std::setfill( ' ' ) << call_counter[i-1] << " | ";
                    COUT << "Millisecs " << std::fixed << std::setw( 13 ) << std::setprecision( 0 ) << std::setfill( ' ' ) << (double)average_time[i-1]/1000000 << " |" << std::endl;
                }
            }
            
            void showInfo() {
                COUT << "filename = " << getFilename() << "\tnumber of lines = " << getNumlines() << std::endl;
            }
            
            void tic(int current_line) {
                std::chrono::high_resolution_clock::time_point current_time = std::chrono::high_resolution_clock::now();
                int n = call_counter[current_line-1];
                call_counter[current_line-1] = n+1;
                long long new_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time-previous_time).count();
                average_time[current_line-1] = (average_time[current_line-1]*n+new_duration)/(n+1);
                previous_time = current_time;
            }
            
            void output(bool annotate = true, bool print = true, int type = 2) {

                std::ios_base::fmtflags f( COUT.flags() );

                // define time measurement for TIC_output
                // 0.. nanoseconds, 1.. microseconds, 2.. milliseconds (default), 3.. seconds
                int outints   = 13; // has to be at least 12
                
                // default (type = 2: Millisecs)
                int outdigits = 6;
                int divideby  = 1000000;
                std::string s_name = "Millisecs ";

                if (type == 0) {
                    s_name = "Nanosecs ";
                    divideby  = 1;
                    outdigits = 0;
                }                
                if (type == 1) {
                    s_name = "Microsecs ";
                    divideby  = 1000;
                    outdigits = 3;
                }
                /*if (type == 2) {
                    s_name = "Millisecs ";
                    divideby  = 1000000;
                    outdigits = 6;
                }*/
                if (type == 3) {
                    s_name = "Secs ";
                    divideby  = 1000000000;
                    outdigits = 6;
                }
                
                if (print == true) {
                    COUT << "\nTIC print: " << filename << std::endl;
                    
                    for (int i = 1; i <= numlines; ++i) {
                        if (call_counter[i-1] > 0) {
                            COUT << "Line " << std::fixed << std::setw( 4 ) << std::setprecision( 0 ) << std::setfill( ' ' ) << i << " | ";
                            COUT << "Calls " << std::fixed << std::setw( 10 ) << std::setprecision( 0 ) << std::setfill( ' ' ) << call_counter[i-1] << " | ";
                            COUT << s_name  << std::fixed << std::setw( outints ) << std::setprecision( outdigits ) << std::setfill( ' ' ) << (double)average_time[i-1]/divideby << " |" << std::endl;
                        }
                    }
                }

                if (annotate == true) {

                    std::string outfile = std::string(filename) + "_prf" + std::to_string(profilerid) + ".h";

                    COUT << "\nTIC annotate: " << filename << " --> " << outfile << std::endl;

                    int i = 0;
                    int maxspaces = outints+s_name.length();
                    std::string line;
                    std::ifstream myfile(filename);

                    std::ofstream myoutfile;
                    myoutfile.open (outfile.c_str());

                    if (myfile.is_open()) {
                        while (getline(myfile, line)) {
                            ++i;
                            if (call_counter[i-1] > 0) {
                                myoutfile << "/* Line " << std::fixed << std::setw( 4 ) << std::setprecision( 0 ) << std::setfill( ' ' ) << i << " | ";
                                myoutfile << "Calls "  << std::fixed << std::setw( 10 ) << std::setprecision( 0 ) << std::setfill( ' ' ) << call_counter[i-1] << " | ";
                                myoutfile << s_name << std::fixed << std::setw( outints ) << std::setprecision( outdigits ) << std::setfill( ' ' ) << (double)average_time[i-1]/divideby << " | */ ";
                                myoutfile << line << std::endl;
                            } else {
                                myoutfile << "/* Line " << std::fixed << std::setw( 4 ) << std::setprecision( 0 ) << std::setfill( ' ' ) << i << " |                  |  ";
                                for (int i=0; i < maxspaces; i++) {
                                    myoutfile << " "; 
                                }
                                myoutfile << "| */ " << line << std::endl;
                            }
                        }
                        myfile.close();
                        time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                        std::string str_end_time = ctime(&end_time);
                        str_end_time.erase(str_end_time.find('\n', 0), 1); // ctime automatically adds line break which we have to delete first
                        std::string sep_line = "/* --------------------------------------------------------------------------------------------------------- */";
                        myoutfile << sep_line << "\n/*           | Call Counter     | Average Time ";
                        for (int i=0; i < (maxspaces-12); i++) {
                            myoutfile << " "; 
                        }
                        myoutfile << "|  SimpleProfiler run at " << str_end_time;
                        for (int i=0; i < (10-int(s_name.length())); i++) {
                            myoutfile << " "; 
                        }
                        myoutfile << "  */\n" << sep_line;
                        myoutfile.close();
                    }
                }

                COUT.flags( f );

            }
        
            static int readNumlines(std::string in_filename) {
                int in_numlines = 0;
                std::string line;
                std::ifstream myfile(in_filename.c_str());
                if (myfile.is_open()) {
                    while (getline(myfile, line)) ++in_numlines;
                    myfile.close();
                } else {
                    COUT << "\nError [SimpleProfiler]: Unable to open file " << in_filename << std::endl;
                    return -1;
                }
                return in_numlines;
            }
    };

    int SimpleProfiler::ObjCounter = 0;
    
    // some macros for predefined simple profiler
    #define TIC_INIT SimpleProfiler myprofiler1(THIS_FILE);
    #define TIC myprofiler1.tic(__LINE__);
    #define TIC_ANNOTATE myprofiler1.output(true, false, 2);
    #define TIC_ANNOTATE_PRINT myprofiler1.output(true, true, 2);
    #define TIC_PRINT myprofiler1.output(false, true, 2);

#endif

#ifndef PROFILER_ON
    #define TIC_INIT
    #define TIC
    #define TIC_ANNOTATE
    #define TIC_ANNOTATE_PRINT
    #define TIC_PRINT
#endif
