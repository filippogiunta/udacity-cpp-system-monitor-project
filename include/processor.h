#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  long GetPrevIdle() const { return prev_idle_; }
  long GetPrevTotal() const { return prev_total_; }

  void SetPrevIdle(long prev_idle) { prev_idle_ = prev_idle; }
  void SetPrevTotal(long prev_total) { prev_total_ = prev_total; }

 private:
  // NOTE: When first we measure the CPU utilization, we assume the base line
  // values for prev_total_ and prev_idle_ are 0.
  long prev_idle_ = 0;
  long prev_total_ = 0;
};

#endif