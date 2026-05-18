#include <vector>
#include <unordered_map>
#include <algorithm>
#include "job.h"
#include "machine.h"
#include "priority_greedy.h"

void priority_greedy(std::vector<Job> &jobs, std::vector<Machine> &machines){
  std::sort(jobs.begin(), jobs.end(), [](const Job &a, const Job &b) {
    if (a.priority != b.priority) return a.priority > b.priority;
    return a.arrival_time < b.arrival_time;
  });

  std::unordered_map<int, Machine*> machine_map;
  for (Machine &m : machines) {
    machine_map[m.id] = &m;
  }

  for (size_t i = 0; i < jobs.size(); i++){
    for (size_t j = 0; j < jobs[i].tasks.size(); j++){
      Machine *machine = machine_map[jobs[i].tasks[j].machine_id];  
      jobs[i].tasks[j].start_time = std::max(machine->available_at, (j > 0 ? jobs[i].tasks[j-1].end_time : jobs[i].arrival_time)); 
      jobs[i].tasks[j].end_time = jobs[i].tasks[j].start_time + jobs[i].tasks[j].process_time;
      jobs[i].tasks[j].remaining_time = jobs[i].tasks[j].process_time;
      machine->available_at = jobs[i].tasks[j].end_time;
    }
  }
}

