#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process();
  Process(int pid);                        // Adding a constructor to the class Process
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization();                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator< (Process const& a) const ;  
  void CalculateCpuUtilization();
  
 private:
 int pid_{0};
 float cpu_usage_{0.0};
};

#endif