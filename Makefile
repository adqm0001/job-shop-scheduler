all:
	g++ -std=c++17 -Wall -o scheduler main.cpp greedy.cpp priority_greedy.cpp preemptive.cpp simulated_annealing.cpp
clean:
	rm -f scheduler
