all:
	g++ -std=c++17 -Wall -o scheduler main.cpp greedy.cpp

clean:
	rm -f scheduler
