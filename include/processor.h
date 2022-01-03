#ifndef PROCESSOR_H
#define PROCESSOR_H

#include<array>
#include<vector>

using std::vector;
using std::array;

class Processor {
 public:
  Processor();
  float Utilization();
  long getTotalJiffies();

 private:
   void CalculateDeltaTime(array<long, 3>&, array<long, 3>&);
   void GetLinuxParsesForJiffies(array<long, 3>&);
   float CalculateUtilization();

   // ActiveJiffies: [0]
   // NonIdleJiffies: [1]
   // TotalJiffies [2]
   array<long, 3> current_cpu_jiffies_;
   array<long, 3> prev_cpu_jiffies_;
   array<float, 3> delta_jiffies_;
};

#endif