#include "greedy.h"
#include <iostream>
#include "job.h"
#include <algorithm>
#include "machine.h"
#include "scheduler.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>

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

int main(int argc, char* argv[]){
  if (argc != 3) {
    std::cerr << "Usage is: ./scheduler jobs.csv machines.csv\n";
    return 1;
  }

  Scheduler scheduler;
  
  std::vector<Job> jobs = loadJobs(argv[1]);
  std::vector<Machine> machines = loadMachines(argv[2]);
  for (size_t i = 0; i < jobs.size() ; i++) {
    scheduler.addJob(jobs[i]);
  }
  for (size_t i = 0; i < machines.size(); i++) {
    scheduler.addMachine(machines[i]);
  }

  scheduler.run(greedy);
  jobs = scheduler.getJobs();

  for (size_t i = 0; i < jobs.size(); i++){
    for (size_t j = 0; j < jobs[i].tasks.size(); j++){
      std::cout << "Job: " << i + 1 << " | Task: " << j + 1 << " | Machine: " << jobs[i].tasks[j].machine_id << " | Start: " << jobs[i].tasks[j].start_time << " | End: " << jobs[i].tasks[j].end_time << "\n";
    }
  } 
  return 0;
}
