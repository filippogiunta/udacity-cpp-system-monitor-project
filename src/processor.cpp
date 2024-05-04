#include "processor.h"

#include <iostream>
#include <vector>

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector<std::string> cpu_values_str = LinuxParser::CpuUtilization();

  std::vector<long> cpu_values{};

  // Convert the string vector to a long vector
  for (std::string cpu_util : cpu_values_str) {
    cpu_values.push_back(std::stol(cpu_util));
  }

  // From
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

  // Idle = idle + iowait
  long idle_cpu =
      cpu_values[LinuxParser::kIdle_] + cpu_values[LinuxParser::kIOwait_];

  // NonIdle = user + nice + system + irq + softirq + steal
  long not_idle_cpu =
      cpu_values[LinuxParser::kUser_] + cpu_values[LinuxParser::kNice_] +
      cpu_values[LinuxParser::kSystem_] + cpu_values[LinuxParser::kIdle_] +
      cpu_values[LinuxParser::kIOwait_] + cpu_values[LinuxParser::kIRQ_] +
      cpu_values[LinuxParser::kSoftIRQ_] + cpu_values[LinuxParser::kSteal_];

  // Total = Idle + NonIdle
  long total_cpu = idle_cpu + not_idle_cpu;

  // totald
  long difference_total_previous_total = total_cpu - GetPrevTotal();
  // idled
  long difference_idle_previous_idle = idle_cpu - GetPrevIdle();

  // Update the prev_total_ and prev_idle_ values for the next measurements
  SetPrevTotal(total_cpu);
  SetPrevIdle(idle_cpu);

  return (float)(difference_total_previous_total -
                 difference_idle_previous_idle) /
         difference_total_previous_total;
}