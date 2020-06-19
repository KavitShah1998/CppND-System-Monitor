#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  

 private:
  double total_time_{0.0};
  double idle_time_{0.0};
  double non_idle_time_{0.0};

  double prev_total_time_{0.0};
  double prev_idle_time_{0.0};
  double prev_non_idle_time_{0.0};
};

#endif