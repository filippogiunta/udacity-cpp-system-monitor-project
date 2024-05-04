#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;

  // The following two variables are initialized with -1.0 to be able to check
  // if the values have been read from the file. If the values are read from the
  // file, then the variable will have positive values (I cannot have negative
  // memory values).
  float total_memory = -1.0;
  float free_memory = -1.0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream linestream(line);

      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          total_memory = std::stof(value);
        } else if (key == "MemFree") {
          free_memory = std::stof(value);
        }
        // I am aware that in this case I could use the break statement within
        // the previous if statement. However, by doing so, I would rely on the
        // fact that "MemTotal" is always before "MemFree" in the file. Which is
        // in this case is true and totally fine. However, as general rule when
        // writing code I prefer to avoid relying on such assumptions.
        if (total_memory != -1.0 && free_memory != -1.0) {
          break;
        }
      }
    }
  }

  // Total used memory = (memTotal - MemFree) / memTotal
  return ((total_memory - free_memory) / total_memory);
}
// TODO: Read and return the system uptime
long int LinuxParser::SystemUpTime() {
  string line;
  string value_1;
  string value_2;

  long uptime = 0;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);

    std::istringstream linestream(line);

    linestream >> value_1 >> value_2;

    uptime = std::stol(value_1);
  }

  return uptime;
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::GetProcStatValues(int pid) {
  std::string filename = kProcDirectory + to_string(pid) + kStatFilename;
  std::ifstream filestream(filename);
  std::string line, value;
  std::vector<std::string> values;

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }

  // Source:
  // https://stackoverflow.com/questions/39066998/what-are-the-meaning-of-values-at-proc-pid-stat
  // According to a generic /proc/[pid]/stat, considering an indexing starting
  // from 0, the values for the active jiffies are in the following positions:
  // 13, 14, 15, 16 (utime, stime, cutime and cstime)

  return (std::stol(values[13]) + std::stol(values[14]) +
          std::stol(values[15]) + std::stol(values[16]));
}

std::string LinuxParser::GetLineFromFile(std::string file, std::string key) {
  std::string line;
  std::string keyFromFile;
  std::string value;

  std::string result;

  std::ifstream filestream(file);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> keyFromFile;
      if (keyFromFile == key) {
        // We do not return the key, but the rest of the line
        std::getline(linestream, result);
        break;
      }
    }
  }
  return result;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  // The values are in the following order (according to /proc/stat):
  // {"user", "nice", "system", "idle", "iowait", "irq", "softirq", "steal",
  // "guest", "guest_nice"}
  vector<string> cpu_values;

  std::string line =
      LinuxParser::GetLineFromFile(kProcDirectory + kStatFilename, "cpu");

  std::istringstream linestream(line);

  std::string value;

  while (linestream >> value) {
    cpu_values.push_back(value);
  }

  return cpu_values;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::string line =
      LinuxParser::GetLineFromFile(kProcDirectory + kStatFilename, "processes");
  std::istringstream linestream(line);
  int value;
  if (linestream >> value) {
    return value;
  }
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line = LinuxParser::GetLineFromFile(
      kProcDirectory + kStatFilename, "procs_running");
  std::istringstream linestream(line);
  int value;
  if (linestream >> value) {
    return value;
  }
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string cmd_line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, cmd_line);
    return cmd_line;
  }
  return cmd_line;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::string line = LinuxParser::GetLineFromFile(
      // I believe that VmRSS is the most accurate value for the RAM used by a
      // process. VmSize is the total virtual memory used by the process, which
      // is not necessarily all in RAM. Indeed VmSize can be larger than the
      // physical memory.
      kProcDirectory + to_string(pid) + kStatusFilename, "VmRSS:");
  std::istringstream linestream(line);
  string value;
  float ram = 0.0f;

  if (linestream >> value) {
    ram = std::round(std::stof(value) / 1024 * 100) / 100;  // Convert KB to MB
  }

  std::ostringstream ram_stream;
  ram_stream << std::fixed << std::setprecision(2) << ram;
  return ram_stream.str();
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string line = LinuxParser::GetLineFromFile(
      kProcDirectory + to_string(pid) + kStatusFilename, "Uid:");
  std::istringstream linestream(line);
  string value;
  if (linestream >> value) {
    return value;
  }
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, username, x, uid_string;
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> x >> uid_string;
      if (uid_string == uid) {
        return username.substr(0, 5);
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  // Reference:
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  long starttime;
  string line, value;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; ++i) {
      linestream >> value;
      // The starttime is the 22nd value in the file (indexing starts from 0)
      if (i == 21) {
        starttime = std::stol(value);
        break;
      }
    }
  }

  long uptime_process =
      LinuxParser::SystemUpTime() - starttime / sysconf(_SC_CLK_TCK);
  return uptime_process;
}