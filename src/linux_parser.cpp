#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <numeric>
#include "linux_parser.h"

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
#include<iostream>
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
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  string key, unit, line;
  float value;
  vector<float> memory_data{};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    for (int i = 0; i < 5; ++i) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> value >> unit;
      memory_data.emplace_back(value);
    }
  }
  float utilization = (memory_data.at(MemoryType::kMemTotal_) -
                       memory_data.at(MemoryType::kMemFree_) -
                       memory_data.at(MemoryType::kMemBuffer_) -
                       memory_data.at(MemoryType::kMemCached_)) /
                       memory_data.at(MemoryType::kMemTotal_);

  return utilization;
}

long LinuxParser::UpTime() { 
  string line;
  long value{0};
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
    return value;
  }
  return value;
}

long LinuxParser::Jiffies(const vector<int>& cpu_data) {
  return std::accumulate(cpu_data.begin(), cpu_data.end(), 0);
}

long LinuxParser::ActiveJiffies(int pid) {
  string key, line;
  long int value{0}, active_jiffies{0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; ++i) {
      linestream >> key;
    }
    for (int i = 0; i < 4; ++i) {
      linestream >> value;
      active_jiffies += value;
    }
    return active_jiffies;
  }
  return active_jiffies;
}

long LinuxParser::ActiveJiffies(const vector<int>& cpu_data) {
  return (cpu_data.at(CPUStates::kUser_) + cpu_data.at(CPUStates::kNice_) +
          cpu_data.at(CPUStates::kSystem_) + cpu_data.at(CPUStates::kIRQ_) +
          cpu_data.at(CPUStates::kSoftIRQ_) + cpu_data.at(CPUStates::kSteal_));
}

long LinuxParser::IdleJiffies(const vector<int>& cpu_data) { 
  return cpu_data.at(CPUStates::kIdle_) + cpu_data.at(CPUStates::kIOwait_);
}

vector<int> LinuxParser::JiffiesData() {
  string line, cpu_string;
  long value;
  vector<int> cpu_data{};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu_string;
    while (linestream >> value) {
      cpu_data.emplace_back(value);
    }
  }
  return cpu_data;
}

vector<string> LinuxParser::CpuUtilization() { return {}; }

int LinuxParser::TotalProcesses() { 
  string key, line;
  float value{0.0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

int LinuxParser::RunningProcesses() { 
  string key, line;
  float value{0.0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line{};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return line;
}

// Read and return the memory used by a process
float LinuxParser::Ram(int pid) {
  const float kb_to_mb = 1000;
  string key, line;
  float value{0.0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return value / kb_to_mb;
        }
      }
    }
  }
  return value;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key, line, value{};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, key, value{};
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      while (linestream >> value >> key) {
        if (key == uid) {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string key, line;
  long int value{0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; ++i) {
      linestream >> key;
    }
    linestream >> value;
    value = value / sysconf(_SC_CLK_TCK);
    return value;
  }
  return value;
}