#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>
#include <vector>
using std::vector;

namespace LinuxParser {
// Filters
const std::string filterProcesses("processes");
const std::string filterRunningProcesses("procs_running");
const std::string filterMemTotalString("MemTotal:");
const std::string filterMemFreeString("MemFree:");
const std::string filterMemBufferString("Buffers:");
const std::string filterMemCachedString("Cached:");
const std::string filterUID("Uid:");
const std::string filterProcMem("VmRSS:");

// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

enum CPUTime {
  kIdleTime_ = 0,
  kActiveTime_,
  kTotalTime_
};

std::vector<std::string> CpuUtilization();
long Jiffies(const vector<int>&);
long ActiveJiffies(const vector<int>&);
long ActiveJiffies(int pid);
long IdleJiffies(const vector<int>&);
vector<int> JiffiesData();

// Processes
std::string Command(int pid);
float Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif