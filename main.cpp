#include "greedy.h"
#include "priority_greedy.h"
#include "preemptive.h"
#include "simulated_annealing.h"
#include <iostream>
#include "job.h"
#include "machine.h"
#include "scheduler.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <chrono>

std::vector<Job> loadJobs(const std::string &path){
  std::ifstream file(path);
  if (!file.good()){
    std::cerr << "File was not found.";
    return {};
  }
  std::string line;
  std::unordered_map<int, Job> jobs_map;
  std::vector<Job> jobs;
  bool header = true;
  while (std::getline(file, line)) {
    if (header){
      header = false;
      continue;
    }
    std::stringstream ss(line);
    std::string token;

    std::getline(ss, token, ','); 
    int job_id = std::stoi(token);

    std::getline(ss, token, ',');
    Priority priority = (token == "NORMAL") ? Priority::NORMAL : Priority::URGENT;

    std::getline(ss, token, ','); 
    int arrival_time = std::stoi(token);

    std::getline(ss, token, ','); 
    int machine_id = std::stoi(token);

    std::getline(ss, token, ',');
    int process_time = std::stoi(token);
    
    if (jobs_map.count(job_id) > 0){
      jobs_map[job_id].tasks.push_back({machine_id, process_time, -1, -1}); 
    } else {
      jobs_map[job_id] = {job_id, priority, arrival_time, {{machine_id, process_time, -1, -1}}};
    }
  }
  for (std::pair<const int, Job> &pair : jobs_map) {
    jobs.push_back(pair.second);
  }
  std::sort(jobs.begin(), jobs.end(), [](const Job &a, const Job &b) {
    return a.arrival_time < b.arrival_time;
  });
  return jobs;
}

std::vector<Machine> loadMachines(const std::string &path){
  std::ifstream file(path);
  if (!file.good()){
    std::cerr << "File was not found.";
    return {};
  }

  std::string line;
  std::vector<Machine> machines;
  bool header = true;
  while (std::getline(file, line)){
    if (header){
      header = false;
      continue;
    }

    int id = std::stoi(line);
    machines.push_back({id, 0, {}});
  }
  return machines;
}

void printJobs(const std::vector<Job> &jobs){
  for (size_t i = 0; i < jobs.size(); i++)
    for (size_t j = 0; j < jobs[i].tasks.size(); j++)
      std::cout << "Job: " << jobs[i].id << " | Task: " << j+1 << " | Machine: " << jobs[i].tasks[j].machine_id << " | Start: " << jobs[i].tasks[j].start_time << " | End: " << jobs[i].tasks[j].end_time << "\n";
}

int main(int argc, char* argv[]){
  if (argc != 3) {
    std::cerr << "Usage is: ./scheduler jobs.csv machines.csv\n";
    return 1;
  }
  
  std::cout << "\n=== GREEDY ===\n";
  Scheduler gs;
  for (Job &j : loadJobs(argv[1])) gs.addJob(j);
  for (Machine &m : loadMachines(argv[2])) gs.addMachine(m);
  auto t1 = std::chrono::high_resolution_clock::now();
  gs.run(greedy);
  auto t2 = std::chrono::high_resolution_clock::now();
  printJobs(gs.getJobs());
  auto greedy_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

  std::cout << "\n=== PRIORITY GREEDY ===\n";
  Scheduler ps;
  for (Job &j : loadJobs(argv[1])) ps.addJob(j);
  for (Machine &m : loadMachines(argv[2])) ps.addMachine(m);
  auto t3 = std::chrono::high_resolution_clock::now();
  ps.run(priority_greedy);
  auto t4 = std::chrono::high_resolution_clock::now();
  printJobs(ps.getJobs());
  auto pg_us = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();

  std::cout << "\n=== PREEMPTIVE ===\n";
  Scheduler pre;
  for (Job &j : loadJobs(argv[1])) pre.addJob(j);
  for (Machine &m : loadMachines(argv[2])) pre.addMachine(m);
  auto t5 = std::chrono::high_resolution_clock::now();
  pre.run(preemptive);
  auto t6 = std::chrono::high_resolution_clock::now();
  printJobs(pre.getJobs());
  auto pre_us = std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5).count();

  std::cout << "\n=== SIMULATED ANNEALING ===\n";
  Scheduler sa;
  for (Job &j : loadJobs(argv[1])) sa.addJob(j);
  for (Machine &m : loadMachines(argv[2])) sa.addMachine(m);
  auto t7 = std::chrono::high_resolution_clock::now();
  sa.run(simulated_annealing);
  auto t8 = std::chrono::high_resolution_clock::now();
  printJobs(sa.getJobs());
  auto sa_us = std::chrono::duration_cast<std::chrono::microseconds>(t8 - t7).count();

  int makespan_greedy = 0, makespan_pg = 0, makespan_pre = 0, makespan_sa = 0;
  for (const Machine &m : gs.getMachines()) makespan_greedy = std::max(makespan_greedy, m.available_at);
  for (const Machine &m : ps.getMachines()) makespan_pg = std::max(makespan_pg, m.available_at);
  for (const Machine &m : pre.getMachines()) makespan_pre = std::max(makespan_pre, m.available_at);
  for (const Machine &m : sa.getMachines()) makespan_sa = std::max(makespan_sa, m.available_at);

  std::cout << "\n";
  std::cout << std::left << std::setw(25) << "Algorithm" << std::setw(12) << "Makespan" << std::setw(15) << "Runtime (us)" << "\n";
  std::cout << std::string(52, '-') << "\n";
  std::cout << std::setw(25) << "Greedy" << std::setw(12) << makespan_greedy << std::setw(15) << greedy_us << "\n";
  std::cout << std::setw(25) << "Priority Greedy" << std::setw(12) << makespan_pg << std::setw(15) << pg_us << "\n";
  std::cout << std::setw(25) << "Preemptive" << std::setw(12) << makespan_pre << std::setw(15) << pre_us << "\n";
  std::cout << std::setw(25) << "Simulated Annealing" << std::setw(12) << makespan_sa << std::setw(15) << sa_us << "\n";

  return 0;
}
