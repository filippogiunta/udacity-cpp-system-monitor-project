#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid) : pid_(pid){};

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {

  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
 
  long process_cpu_stat = LinuxParser::GetProcStatValues(Pid());
  long process_uptime = LinuxParser::UpTime(Pid());
  cpu_utilization_ = 100 * (static_cast<float>(process_cpu_stat) /
                            sysconf(_SC_CLK_TCK) / process_uptime);
  return cpu_utilization_;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return a.cpu_utilization_ < cpu_utilization_;
}