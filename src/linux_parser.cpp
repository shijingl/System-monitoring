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

template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line, key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
};

template <typename T>
T getValueOfFile(std::string const &filename) {
  std::string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};

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
  float total = findValueByKey<float>(filterMemTotalString, kMeminfoFilename);
  float free = findValueByKey<float>(filterMemFreeString, kMeminfoFilename);
  float buffer = findValueByKey<float>(filterMemBufferString, kMeminfoFilename);
  float cached = findValueByKey<float>(filterMemCachedString, kMeminfoFilename);
  return (total - free - buffer - cached) / total;
}

long LinuxParser::UpTime() { 
  return getValueOfFile<long>(kUptimeFilename);
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
    // pop the first value
    linestream >> cpu_string;
    while (linestream >> value) {
      cpu_data.emplace_back(value);
    }
  }
  return cpu_data;
}

int LinuxParser::TotalProcesses() {
  return findValueByKey<float>(filterProcesses, kStatFilename);
}

int LinuxParser::RunningProcesses() {
  return findValueByKey<float>(filterRunningProcesses, kStatFilename);
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
  const float kb_to_mb = 1024;
  // use VmRSS because VmRSS is the exact physical memory size 
  // VmSize is the sum of all the virtual memory, which is more than Physical RAM size.
  float vmRSS = findValueByKey<float>(filterProcMem, std::to_string(pid) + kStatusFilename);
  return vmRSS / kb_to_mb;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  return findValueByKey<std::string>(filterUID, std::to_string(pid) + kStatusFilename);
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