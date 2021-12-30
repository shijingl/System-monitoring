#include "processor.h"
#include "linux_parser.h"
#include<vector>
using LinuxParser::CPUTime;
using std::vector;
using std::array;

Processor::Processor() { GetLinuxParsesForJiffies(prev_cpu_jiffies_); }

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    GetLinuxParsesForJiffies(current_cpu_jiffies_);
    CalculateDeltaTime(current_cpu_jiffies_, prev_cpu_jiffies_);
    const float utilization = CalculateUtilization();
    current_cpu_jiffies_.swap(prev_cpu_jiffies_);
    return utilization;
}

void Processor::GetLinuxParsesForJiffies(array<long, 3>& cpu_jiffies) {
    vector<int> cpu_data = LinuxParser::JiffiesData();
    cpu_jiffies[CPUTime::kIdleTime_] = LinuxParser::IdleJiffies(cpu_data);
    cpu_jiffies[CPUTime::kActiveTime_] = LinuxParser::ActiveJiffies(cpu_data);
    cpu_jiffies[CPUTime::kTotalTime_] = LinuxParser::Jiffies(cpu_data);
}

void Processor::CalculateDeltaTime(array<long, 3>& first_cpu_jiffies,
                                   array<long, 3>& second_cpu_jiffies) {
  for (int i = 0; i < 3; ++i) {
    delta_jiffies_[i] = first_cpu_jiffies[i] - second_cpu_jiffies[i];
  }
}

float Processor::CalculateUtilization() {
  // small_number_buffer to prevent divide by zeros
  const float small_number_buffer = 0.0000000001;
  const float utilization =
      delta_jiffies_[CPUTime::kActiveTime_] /
      (delta_jiffies_[CPUTime::kTotalTime_] + small_number_buffer);
  return utilization;
}

/*
long Processor::getTotalJiffies() {
    return current_cpu_jiffies_[CPUTime::kTotalTime_];
}
*/