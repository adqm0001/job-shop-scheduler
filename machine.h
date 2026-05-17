#ifndef MACHINE_H
#define MACHINE_H

#include <vector>
#include "job.h"

class Machine {
  public:
    int id;
    int available_at = 0;
    std::vector<Task> tasks;
};

#endif
