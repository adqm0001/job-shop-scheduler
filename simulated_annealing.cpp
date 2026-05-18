#include <vector>
#include <unordered_map>
#include "job.h"
#include "machine.h"
#include "simulated_annealing.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

int calculate_makespan(std::vector<Job> jobs, std::vector<Machine> machines){
  std::unordered_map<int, Machine*> machine_map;
  for (Machine &m : machines) {
    machine_map[m.id] = &m;
  }

  int makespan = -1;

  for (size_t i = 0; i < jobs.size(); i++){
    for (size_t j = 0; j < jobs[i].tasks.size(); j++){
      Machine *machine = machine_map[jobs[i].tasks[j].machine_id];  
      jobs[i].tasks[j].start_time = std::max(machine->available_at, (j > 0 ? jobs[i].tasks[j-1].end_time : jobs[i].arrival_time)); 
      jobs[i].tasks[j].end_time = jobs[i].tasks[j].start_time + jobs[i].tasks[j].process_time;
      jobs[i].tasks[j].remaining_time = jobs[i].tasks[j].process_time;
      machine->available_at = jobs[i].tasks[j].end_time;
      makespan = std::max(machine->available_at, makespan);
    }
  }
  return makespan;
}

void simulated_annealing(std::vector<Job> &jobs, std::vector<Machine> &machines){
  double T = 1000.0;
  double cooling = 0.995;
  double T_min = 0.1;
  std::srand(std::time(0));

  int current_makespan = calculate_makespan(jobs, machines);
  
  while (T > T_min){
    int first_rand_index = std::rand() % jobs.size();
    int second_rand_index = std::rand() % jobs.size();
      
    std::swap(jobs[first_rand_index], jobs[second_rand_index]);
    int new_makespan = calculate_makespan(jobs, machines);
    if (current_makespan < new_makespan){
      if ((double)std::rand() / RAND_MAX > std::exp(-(new_makespan - current_makespan) / T)){
        std::swap(jobs[first_rand_index], jobs[second_rand_index]);
        T *= cooling;
        continue;
      }
    } 
    current_makespan = new_makespan;
    T *= cooling;
  }
  std::unordered_map<int, Machine*> machine_map;
  for (Machine &m : machines) {
    machine_map[m.id] = &m;
  }

  int makespan = -1;

  for (size_t i = 0; i < jobs.size(); i++){
    for (size_t j = 0; j < jobs[i].tasks.size(); j++){
      Machine *machine = machine_map[jobs[i].tasks[j].machine_id];  
      jobs[i].tasks[j].start_time = std::max(machine->available_at, (j > 0 ? jobs[i].tasks[j-1].end_time : jobs[i].arrival_time)); 
      jobs[i].tasks[j].end_time = jobs[i].tasks[j].start_time + jobs[i].tasks[j].process_time;
      jobs[i].tasks[j].remaining_time = jobs[i].tasks[j].process_time;
      machine->available_at = jobs[i].tasks[j].end_time;
      makespan = std::max(machine->available_at, makespan);
    }
  }
}

