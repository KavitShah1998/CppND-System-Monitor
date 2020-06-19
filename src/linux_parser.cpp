#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "linux_parser.h"
#include <iomanip>
#include <iostream>
using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::unordered_map<string,int> mem_data_keys{{"MemTotal",0}, {"MemFree",1}, {"Buffers",2} , {"Cached",3}, {"SReclaimable",4}, {"Shmem",5}, {"SwapTotal",6}, {"SwapFree",7}};  // the HashMap data is (key): (index for value to be searched in "mem_data_values") This would also simplify searching in the while loop below
  std::vector<double> mem_data_values(8, 0);

  double total_mem_used {0}, non_cached_OR_buffer_mem{0}, buffers{0}, cached_mem{0}, swap{0};
  float perc_mem_used{0.0};
  string line{""};

  std::ifstream mem_stream(kProcDirectory + kMeminfoFilename);
  string key{""};
  long value {0};
  if(mem_stream.is_open()){
    std::unordered_map<string, int> :: iterator it = mem_data_keys.begin();

    while(std::getline(mem_stream, line)){
      
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream mem_line_stream(line);
      
      mem_line_stream >> key >> value;
      it = mem_data_keys.find(key);
      if(it!=mem_data_keys.end())
        mem_data_values[(it->second)] = value; 
    }
  }
  else {
    std::cout << " Error reading file\n ";
  }

  total_mem_used = (mem_data_values[0] - mem_data_values[1])/1.0;
  buffers = (mem_data_values[2])/1.0;
  cached_mem = (mem_data_values[3] + mem_data_values[4] - mem_data_values[5])/1.0;
  swap = (mem_data_values[6] - mem_data_values[7])/1.0;
  non_cached_OR_buffer_mem = total_mem_used - (buffers + cached_mem);
  perc_mem_used =  (total_mem_used / mem_data_values[0]);

  return perc_mem_used;
 }

// Read and return the system uptime
long LinuxParser::UpTime() { 
  double uptime{0};
  double idletime{0};
  std::ifstream uptime_stream(kProcDirectory + kUptimeFilename);
  std::string line{""};
  if(uptime_stream.is_open()){
    std::getline(uptime_stream, line);
    std::istringstream line_uptime_stream (line);
    line_uptime_stream >> uptime >> idletime ;
  }
  return uptime;
 }

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string line{""};
  string cpu;
  
  long total_jiffies{0};
  std::vector<long> jiffies_vector{};
  std::ifstream cpu_stream(kProcDirectory + kStatFilename);
  if(cpu_stream.is_open()){
    std::getline(cpu_stream, line);
    std::istringstream cpu_line_stream(line);
    cpu_line_stream >> cpu;
    for(int i=1;i<=10;i++){
      long cpu_jiffies{0};
      cpu_line_stream >> cpu_jiffies;
      total_jiffies +=cpu_jiffies;    }
  }

 return total_jiffies;
}

// Read and return the number of active jiffies for a PID
std::vector<long> LinuxParser::JiffyData(int pid) { 
  long total_time{0};
  string line {""};
  long start_time{0};
  std::vector<long> time_data{};
  std::ifstream act_jiff_stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(act_jiff_stream.is_open()){
    std::getline(act_jiff_stream,line);

    std::istringstream act_jiff_line_stream(line);
    for(int i=0;i<=21;i++){
      string data{""};
      act_jiff_line_stream >> data;
      if (i==13 || i==14 || i==15 || i==16){
        total_time += std::stol(data, nullptr, 10);
      }
      else if(i==21){
        start_time = std::stol(data);
      }
    }
  }
  else{
    std::cout << "File not opened \n";
  }

  time_data.push_back(total_time);
  time_data.push_back(start_time);

  return time_data;
 }

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::ifstream active_jiffs_stream(kProcDirectory + kStatFilename);
  string line{""};
  long jiffies{0};

  string cpu{""};
  std::vector<long> cpu_data{};
  if(active_jiffs_stream.is_open()){
    std::getline(active_jiffs_stream,line);
    std::istringstream line_act_jiffs_stream(line);

    line_act_jiffs_stream >> cpu;
    for(int i=0;i<10;i++){
      if(i!=3 && i!=4){
        long data{0};
        line_act_jiffs_stream >> data;
        cpu_data.push_back(data);
      }
      else{
        long thrash{0};
        line_act_jiffs_stream >> thrash;
      }
    }
  }

// accumulate the jiffies data
  for (auto i=0;i<cpu_data.size();i++){
    jiffies +=cpu_data[i];
  }
  return jiffies;
 }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string cpu{""};
  string line{""};
  long idle_jiffies{0};
  std::vector<long> cpu_jiffies{};
  std::ifstream idlejiffies_stream(kProcDirectory + kStatFilename);

  if(idlejiffies_stream.is_open()){
    std::getline(idlejiffies_stream,line);
    std::istringstream line_idle_stream(line);
    line_idle_stream >> cpu;
    for(int i=0; i <=4 ; i ++) {
      if(i==3 || i==4){
        long data{0};
        line_idle_stream >> data;
        cpu_jiffies.push_back(data);
      }
      else{
        long thrash{0};
        line_idle_stream >> thrash;
      }
    }

    for(auto i=0; i<cpu_jiffies.size(); i++){
      idle_jiffies +=cpu_jiffies[i];
    }
    return idle_jiffies;
  }
 }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::ifstream cpu_stream(kProcDirectory + kStatFilename);

  std::vector<string> cpu_jiffies{};
  std::string cpu{""};
  std::string line{""};

  if(cpu_stream.is_open()){
    std::getline(cpu_stream,line);
    std::istringstream cpu_util(line);
    cpu_util >> cpu;
    int i=0;
    while(i<10){
      string j;
      cpu_util >> j;
      cpu_jiffies.push_back(j);
      i++;
    }
  }
  return cpu_jiffies;
 }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line{""};

  std::ifstream total_procs_stream(kProcDirectory + kStatFilename);
  string key {""};
  int value{0};
  if(total_procs_stream.is_open()){
    while(std::getline(total_procs_stream,line)){
      std::istringstream line_total_procs_stream(line);
      line_total_procs_stream >> key;
      if(key == "processes"){
        line_total_procs_stream >>value;
        return value;
      }
    }
  }
  return 0; // to remove the warnings
 }

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line{""};
  string key{""};
  int value{1};

  std::ifstream running_procs_stream(kProcDirectory + kStatFilename);
  if(running_procs_stream.is_open()){
    while(std::getline(running_procs_stream,line)){
      std::istringstream line_running_procs_stream(line);
      line_running_procs_stream >> key;
      if(key == "procs_running"){
        line_running_procs_stream >> value;
        return value;
      }
    }
  }
  return 0; // to remove the warnings
 }

// Read and return the command associated with a process
  string LinuxParser::Command(int pid) { 
  std::ifstream cmdline_stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  std::string line {""};
  if(cmdline_stream.is_open()){
    std::getline(cmdline_stream, line);
  }
  return line;
 }

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  std::ifstream ram_stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string line {""};
  
  string search_key {"VmSize"};
  string key{""};
  long value{0};
  if(ram_stream.is_open()){
    while(std::getline(ram_stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream line_ram_stream(line);
      line_ram_stream >> key >> value;
      if(key == search_key){
        return(std::to_string(value / 1024));
      }
    }
  }
  return string(); // to remove the warnings
  }

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  std::ifstream uid_stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string line{""};
  string key{""}, value{""};

  if(uid_stream.is_open()){
    while(std::getline(uid_stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream line_uid_stream(line);
      line_uid_stream >> key >> value;
      if(key == "Uid"){
        return value;
      }
    }
  }
  return string(); // to remove the warnings
 }

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line{""};
  string user{""};
  
  string user_uid = Uid(pid);
  std::ifstream user_Stream(kPasswordPath);
  
  std::string x{"x"} , uid1{"1"}, uid2{"2"};
  if(user_Stream.is_open()){
    while(std::getline(user_Stream,line)){
    std::replace(line.begin(),line.end(), ':', ' ');
    std::istringstream line_user_stream(line);
    line_user_stream >> user >> x >> uid1 >> uid2;
    if(uid1 == user_uid)
      return user;
    }
  }
  return string(); // to remove the warnings
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long uptime(0);

  string line{""};
  string data{""};
  int count = 0;
  std::ifstream proc_uptime_stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if(proc_uptime_stream.is_open()){
    
    std::getline(proc_uptime_stream, line);
    std::istringstream line_uptime_stream(line);
    while(count<=21){
      line_uptime_stream >> data;
      count++;
    }
  }
  uptime = std::stol(data,nullptr,10);
  return uptime;
 }