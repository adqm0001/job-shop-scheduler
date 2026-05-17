#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <functional>
#include "job.h"
#include "machine.h"

class Scheduler {
  private:
    std::vector<Job> jobs;
    std::vector<Machine> machines;
  public: 
    const std::vector<Job>& getJobs() const {
      return jobs;
    }
    const std::vector<Machine>& getMachines() const {
      return machines;
    }
    void addJob(const Job &job){
      jobs.push_back(job);
    }
    void addMachine(const Machine &machine){
      machines.push_back(machine);
    }
    void run(std::function<void(std::vector<Job> &jobs, std::vector<Machine> &machines)> algorithm){
      algorithm(jobs, machines);
    }
};

#endif
