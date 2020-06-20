#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process() {CalculateCpuUtilization();}
Process::Process(int pid) : pid_(pid) {CalculateCpuUtilization();}

//  Return this process's ID
int Process::Pid() { return pid_; }

// Compute CPU Utilization for this Process to update private variables
void Process::CalculateCpuUtilization(){

std::vector<long> cpu_process_data = LinuxParser::JiffyData(pid_);
    double process_total_time = cpu_process_data[0] / 1.0;
    double process_start_time = cpu_process_data[1] / 1.0;
    double uptime = LinuxParser::UpTime() / 1.0;
    double clock_freq = sysconf(_SC_CLK_TCK) / 1.0;
    double total_cpu_working_time = (uptime - (process_start_time / clock_freq)) / 1.0;

    cpu_usage_ = ((process_total_time / clock_freq) / total_cpu_working_time) / 1.0;
}
// Return this process's CPU utilization
float Process::CpuUtilization() { return cpu_usage_; } 

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)

long int Process::UpTime() { 
    long uptime = LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK); 
    return uptime;
    }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }


// Overload the "less than" comparison operator for Process objects
bool Process::operator< ( Process const& a) const {
    double this_cpu = this->cpu_usage_;
    double a_cpu = a.cpu_usage_;
    
    return this_cpu > a_cpu;
}
