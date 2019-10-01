//
// Created by jake on 9/26/19.
//

#ifndef PCB_H_
#define PCB_H_
#include <cstdlib>  //This is for the exit command.
#include <queue>
#include <iostream>
#include <vector>

/*
 *  In the class below, it declares a "queue<etype>" array.
 *  you don't need to use the STL queue (but you can), choose a data structure
 * that you think works.   If you choose to write you own queue class, then
 * it has better follow all the requirements to a queue, to include the O time
 * minimum timing requirements of c++
*/



class pcb
{
public:
   int pid;
   int priority;
   int value;
   int start_time;
   int run_time;
   int cpu_time;
   int quantum;
   
   pcb();
   pcb(int pid, int priority, int value, int start_time, int run_time, int cpu_time);
   void printPcb();
   void setPriority(int);
   ~pcb();
private:
	
};

pcb::pcb(){

pid=0;
priority=0;
value=0;
start_time=0;
run_time=0;
cpu_time=0;
quantum=0;

}
pcb::pcb(int pid1, int priority1, int value1, int start_time1, int run_time1, int cpu_time1){

	this->pid = pid1;
	
	this->value=value1;
	this->start_time=start_time1;
	this->run_time=run_time1;
	this->cpu_time = cpu_time1;
	setPriority(priority1);

}
void pcb::printPcb(){
std::cout<<"pid: "<<pid<<"\n";
std::cout<<"priority: "<<priority<<"\n";
std::cout<<"value: "<<value<<"\n";
std::cout<<"start_time: "<<start_time<<"\n";
std::cout<<"run_time: "<<run_time<<"\n";

}
void pcb::setPriority(int temp){
	this->priority =temp;

	switch(temp)
	{
		case 0: this->quantum = 1;//priority was 0 return 1
		break;

		case 1: this->quantum= 2;//priority was 1 return 2
		break;

		case 2: this->quantum= 4;//priority was 2 return 4
		break;

		case 3: this->quantum= 8;//priority was 0 return 1
		break;

		defualt: std::cout<<"error in assignQuantum in pcb";//priority was 0 return 1
		break;

	}
	
}
// destructor

pcb::~pcb(){

}

#endif
