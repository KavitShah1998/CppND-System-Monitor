#include "processor.h"
#include <vector>
#include <string>
#include "linux_parser.h"
using std::string;


float Processor::Utilization() { 
    std::vector<string> cpu_data_string = LinuxParser::CpuUtilization();
    std::vector<long> cpu_data{};


    float cpu_util{0.0f};

    for(int i=0; i<cpu_data_string.size(); i++){
        cpu_data.push_back(std::stol(cpu_data_string[i], nullptr,10));
    }

    double usertime{cpu_data[0]/1.0};
    double nicetime{cpu_data[1]/1.0};
    double systemalltime{(cpu_data[2] + cpu_data[5] + cpu_data[6])/1.0};
    double steal{cpu_data[7]/1.0};
    double virtalltime{(cpu_data[8] + cpu_data[9])/1.0};

    idle_time_ = cpu_data[3] + cpu_data[4];
    non_idle_time_ = usertime + nicetime + systemalltime + steal;

    total_time_ = idle_time_ + non_idle_time_;

    // cpu_util = (non_idle_time_)/total_time_;  // here the cpu will redline.

    /* The following is done to avoid redlining; */

    //Calculating the difference in values between the two successive intervals
    double total_d = (total_time_ - prev_total_time_)/1.0; 
    double idle_d = (idle_time_ - prev_idle_time_)/1.0;

    // Calculating the cpu_utilization based upon the difference between two successive intervals
    cpu_util = (total_d - idle_d)/total_d;


    // Updating the values of prev interval to store current values which would be used in the next cycle.
    prev_idle_time_ = idle_time_;
    prev_non_idle_time_ = non_idle_time_;
    prev_total_time_ = total_time_;
    
    return cpu_util;
    
 }