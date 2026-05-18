#ifndef JOB_H
#define JOB_H

#include <vector>

enum class Priority { NORMAL, URGENT };

struct Task {
  int machine_id;
  int process_time = -1;
  int start_time = -1;
  int end_time = -1;
  int remaining_time = -1;
};
class Job {
  public:
    int id;
    Priority priority;
    int arrival_time;
    std::vector<Task> tasks;
};
#endif
