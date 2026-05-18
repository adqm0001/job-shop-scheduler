#include <vector>
#include <unordered_set>
#include "job.h"
#include "machine.h"
#include <unordered_map>
#include "preemptive.h"
#include <climits>
#include <optional>
#include <queue>

void preemptive(std::vector<Job> &jobs, std::vector<Machine> &machines){
  auto cmp = [](Job &a, Job &b) {
    if (a.priority != b.priority) return a.priority < b.priority;
    return a.arrival_time > b.arrival_time;
  };
  std::priority_queue<Job, std::vector<Job>, decltype(cmp)> jobs_queue(cmp);
  
  auto machines_busy = [&]() {
    for (Machine &m : machines){
      if (m.current_job_id.has_value()) return true;
    }
    return false;
  };

  std::unordered_map<int, Job*> jobs_map;
  for (Job &j : jobs) {
    jobs_map[j.id] = &j;
  }


  std::unordered_set<int> jobs_added;
  int current_time = 0;

  for (size_t i = 0; i < jobs.size(); i++){
    if (jobs[i].arrival_time == 0){
        jobs_queue.push(jobs[i]);
        jobs_added.insert(jobs[i].id);
    }
  }

  while (!jobs_queue.empty() || machines_busy()){
    
    for (size_t i = 0; i < machines.size(); i++){
      if (machines[i].current_job_id.has_value() && machines[i].available_at <= current_time){ 
        int job_id = machines[i].current_job_id.value();
        int task_index = machines[i].current_task_index.value();
        jobs_map[job_id]->tasks[task_index].remaining_time = 0;
        machines[i].current_task_index = std::nullopt;
        machines[i].current_job_id = std::nullopt;

        bool has_more = false;
        for (size_t t = task_index + 1; t < jobs_map[job_id]->tasks.size(); t++){
          if (jobs_map[job_id]->tasks[t].remaining_time != 0){
            has_more = true;
            break;
          }
        }
        if (has_more) jobs_queue.push(*jobs_map[job_id]);
      }
    }

    for (size_t i = 0; i < jobs.size(); i++){
      if (jobs_added.count(jobs[i].id)) continue;
      if (jobs[i].arrival_time <= current_time){
        jobs_queue.push(jobs[i]);
        jobs_added.insert(jobs[i].id);
      }
    }

    for (size_t i = 0; i < machines.size(); i++){
      if (!machines[i].current_job_id.has_value() && !jobs_queue.empty()){ 
        int current_task = -1;
        for (size_t t = 0; t < jobs_queue.top().tasks.size(); t++){
          if (jobs_queue.top().tasks[t].remaining_time != 0){
            current_task = t;
            break;
          }
        }
        if (current_task != -1 && jobs_queue.top().tasks[current_task].machine_id == machines[i].id){
          machines[i].current_job_id = jobs_queue.top().id;
          machines[i].current_task_index = current_task;
          int job_id = machines[i].current_job_id.value();
          Task &task = jobs_map[job_id]->tasks[current_task];
          task.start_time = current_time;
          task.remaining_time = (task.remaining_time == -1) ? task.process_time : task.remaining_time;
          task.end_time = current_time + task.remaining_time;
          machines[i].available_at = task.end_time;
          jobs_queue.pop();
        }
      }
    }

    for (size_t i = 0; i < machines.size(); i++){
      if (machines[i].current_job_id.has_value() && !jobs_queue.empty()){ 
        if (jobs_map[machines[i].current_job_id.value()]->priority < jobs_queue.top().priority){
          int old_job_id = machines[i].current_job_id.value();
          int old_task_index = machines[i].current_task_index.value(); 
          jobs_map[old_job_id]->tasks[old_task_index].remaining_time -= current_time - jobs_map[old_job_id]->tasks[old_task_index].start_time;
          jobs_queue.push(*jobs_map[old_job_id]);
          machines[i].current_job_id = jobs_queue.top().id;
          jobs_queue.pop();
          
          int new_job_id = machines[i].current_job_id.value();
          for (size_t t = 0; t < jobs_map[new_job_id]->tasks.size(); t++) {
            if (jobs_map[new_job_id]->tasks[t].remaining_time != 0) {
              machines[i].current_task_index = t;
              Task &new_task = jobs_map[new_job_id]->tasks[t];
              new_task.start_time = current_time;
              new_task.remaining_time = (new_task.remaining_time == -1) ? new_task.process_time : new_task.remaining_time;
              new_task.end_time = current_time + new_task.remaining_time;
              machines[i].available_at = new_task.end_time;
              break;
            }
          }
        }
      }
    }

    int next_arrival = INT_MAX;
    int next_completion = INT_MAX;

    for (size_t i = 0; i < jobs.size(); i++){
      if (!jobs_added.count(jobs[i].id)){
      next_arrival = std::min(next_arrival, jobs[i].arrival_time);
      };
    } 

    for (size_t i = 0; i < machines.size(); i++){
      if (machines[i].current_job_id.has_value()){
        next_completion = std::min(next_completion, machines[i].available_at);
      }
    }
    if (next_arrival != INT_MAX || next_completion != INT_MAX){
      current_time = std::min(next_arrival, next_completion);
    }
  }
}

