#ifndef MACHINE_H
#define MACHINE_H

#include <vector>
#include "job.h"
#include <optional>

class Machine {
  public:
    int id;
    int available_at = 0;
    std::optional<int> current_job_id = std::nullopt;
    std::optional<int> current_task_index = std::nullopt;
    std::vector<Task> tasks;
};

#endif
