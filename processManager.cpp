#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "pcb.h"
#include "queue_array.h" 
#define QUANTUM 1
#define CPU 0
using namespace std;

//prototype functions
// block function 

//updates the times and values in pcb
void contextSwitch(int, int, vector<pcb> &pcb);
//moves process from cpu to either ready or blocked
int processTrans(int, int, QueueArray<int>& que, vector<pcb> &pcb);
//scheduler 
void scheduler(int,vector<pcb> &pcb, QueueArray<int>& que);
//print function, handles the format
void printer(vector<pcb> &pcb,vector<int> &cpu, QueueArray<int> &b0,QueueArray<int> &b1,QueueArray<int> &b2, QueueArray<int> &ready,int time);
//starts a process 
void startProcess(vector<int> &cpu, QueueArray<int> &ready, vector<pcb> &pcb);
// BLOCKS processes 
void block(int, vector<int> running, vector<pcb> &pcb,QueueArray<int> &b0,QueueArray<int> &b1,QueueArray<int> &b2 );

/* processManager.cpp file */

int main(int argc, char *argv[]) {
  vector<pcb> pcb_table(100);
  QueueArray <int> readyState(4);
  
  QueueArray <int> r0(50); // blocked on resource 0
  QueueArray <int> r1(50); // blocked on resource 1
  QueueArray <int> r2(50); // blocked on resource 2

  

  //first element is pid, second is always quantum time;
  vector<int> runningState(2);
  int i, max = 100;
  int mcpipe2[2], num;
  char chr;
  int pid,rid,value,run_time,MyTime=0;
  char cmd;
 
  //convert the parameters into the address for the pipe
  mcpipe2[0] = atoi(argv[1]);
  mcpipe2[1] = atoi(argv[2]);

  //read a character from the pipe
  read(mcpipe2[0], &chr, sizeof(char));

  while (chr != 'T') {
    if(chr == 'S'){
	//cmd was s read pid value and run_time
	  read(mcpipe2[0], &pid, sizeof(int));
     // cout <<"\nchild read pid: \n"<<pid<<endl;

	//cmd was s read value
      read(mcpipe2[0], &value, sizeof(int));
     // cout <<"\nchild read value: \n"<<value<<endl;

	//cmd was s read run_time
      read(mcpipe2[0], &run_time, sizeof(int));
     // cout <<"\nchild read run_time: \n"<<run_time<<endl;
	
	//add to the pcb table
	pcb temp(pid,0,value,MyTime,run_time,0);
	pcb_table[pid]=temp;

	
	readyState.Enqueue(pid,0); // add process to ready Queue

	if(runningState[QUANTUM] ==0){ // checks to see if processes needs started 
	startProcess(runningState, readyState, pcb_table);
	}

    }
    else if(chr== 'B'){
	
	//cmd was B read rid
      read(mcpipe2[0], &rid, sizeof(int));
    
	// block the current process for this resouce 
	
	 block(rid, runningState, pcb_table ,r0,r1,r2);
	
	//move new processes in 
    startProcess(runningState, readyState, pcb_table);

    }
    else if(chr== 'U'){

	
	//cmd was U read rid
      read(mcpipe2[0], &rid, sizeof(int));
  //    cout <<"\nchild read rid: \n"<<rid<<endl;

	// need to unblock the processes waiting for this resource
	int temp;
	switch(rid){
	
	case 0: temp= r0.Dequeue();
	break;

	case 1: temp= r1.Dequeue();
	break;	

	case 2: temp= r2.Dequeue();
	break;
	
	default: cout<<"error in unblocking\n";
	break;

	}
	
	//add back to same priority :(
	readyState.Enqueue(temp,pcb_table[temp].priority);
	//scheduler(temp,pcb_table,readyState);
    }
    else if(chr== 'Q'){
	//end of one time unit also needs to check the quantum 
	// first this checks to make sure we have a process started
	
	MyTime++; 
	int quantumTemp = runningState.at(QUANTUM);
		quantumTemp--;
		runningState.at(QUANTUM) = quantumTemp;
		contextSwitch(runningState[CPU], runningState[QUANTUM], pcb_table);
		if(runningState.at(QUANTUM) == 0){
        scheduler(runningState.at(CPU),pcb_table, readyState);
		//off load readyqueue to cpu
		startProcess(runningState, readyState, pcb_table);
		}else{
		
		//quantum wasn't finished, still need to check if process is done;
        //contextSwitch(runningState[CPU], runningState[QUANTUM], pcb_table);
		if(pcb_table[runningState[CPU]].run_time == pcb_table[runningState[CPU]].cpu_time ){
	
						startProcess(runningState, readyState, pcb_table);
			}
		}
    }
    else if(chr== 'C'){
	//cmd was C read cmd and num
		int cmdPid = runningState.at(CPU);
		int valuePid = pcb_table[cmdPid].value;
	  read(mcpipe2[0], &cmd, sizeof(char));
     // cout <<"\nchild read cmd: \n"<<cmd<<endl;

	//cmd was C read num
      read(mcpipe2[0], &num, sizeof(int));
    //  cout <<"\nchild read num: \n"<<num<<endl;

	// now we need to handle these commands and then increment time;
		switch(cmd)
	{
		case 'A': valuePid= valuePid+num;
		break;

		case 'S':  valuePid= valuePid-num;
		break;

		case 'M':  valuePid= valuePid*num;
		break;

		case 'D':	 valuePid= valuePid/num;
		break;

		defualt: cout<<"error in CMD switch statement";
		break;

	}
		//assign new value
		pcb_table[cmdPid].value = valuePid;
	
		//handle time
		
      MyTime++; 
	    int quantumTemp = runningState.at(QUANTUM);
		quantumTemp--;
		runningState.at(QUANTUM) = quantumTemp;
		contextSwitch(runningState[CPU], runningState[QUANTUM], pcb_table);
		if(runningState.at(QUANTUM) == 0){
		//then switch processes because its done;
		//update the run time 
		//contextSwitch(runningState.at(CPU), 0, pcb_table);
		//process may not be finished, if not, put back in queue
        scheduler(runningState.at(CPU),pcb_table, readyState);
		//off load readyqueue to cpu
		startProcess(runningState, readyState, pcb_table);
		}else{
		
		//quantum wasn't finished, still need to check if process is done;
        //contextSwitch(runningState[CPU], runningState[QUANTUM], pcb_table);
		if(pcb_table[runningState[CPU]].run_time <= pcb_table[runningState[CPU]].cpu_time ){
						startProcess(runningState, readyState, pcb_table);
			}
		}

    }
    else if(chr== 'P'){
	
	// this should trigger the fork, needs to print state
	if(fork() == 0) 
        { 
		printer(pcb_table ,runningState , r0, r1, r2 , readyState, MyTime);
		exit(0);
		}
	
    }
  
    else{
	std::cout<<"something happens in my input, error\n";
	exit(2);
    }

   //read a character from the pipe
    read(mcpipe2[0], &chr, sizeof(char));
  }
  //close down the pipe 
  close(mcpipe2[0]);
  close(mcpipe2[1]);
  //return with a 2, so the parent receive the status message of 2
  // note the number can not greater then 255.
  return 2;
}

void contextSwitch(int pid, int quantumPassed, std::vector<pcb> &pcb){
//handles updating the pcb so we need pid, and quantum, if this called then its pulled early so assuming that quantum != 0;

	//pcb[pid].cpu_time += ((pcb[pid].quantum)-quantumPassed);
	pcb[pid].cpu_time++;
	//now cpu_time is up to date 
	
	
}

void scheduler(int pid,vector<pcb> &pcb, QueueArray<int>& que){
	//first check process to see if its finished 
	
	if(pcb[pid].cpu_time >= pcb[pid].run_time ){
	// yes process is finished, dont add to ready queue 
	}else
	{
		// process is not finished
	int processPrior = pcb[pid].priority;	

	// if priority is less than the greatest 3 
	if(processPrior < 3){
	//move priority up
	processPrior++;
	pcb[pid].setPriority(processPrior);
	que.Enqueue(pid,processPrior);
	}else{
	//the process was already priority 3 so add back to ready queue and reset quantum

	pcb[pid].setPriority(3);
	que.Enqueue(pid,3);
	}
	}

	
}
void printer(vector<pcb> &pcb,vector<int> &cpu, QueueArray<int> &b0,QueueArray<int> &b1,QueueArray<int> &b2, QueueArray<int> &ready,int time){
int *b00 = b0.Qstate(0); // this is for blocked state 0 priority 0
int *b01 = b0.Qstate(1); // this is for blocked state 0 priority 1
int *b02 = b0.Qstate(2); // this is for blocked state 0 priority 2
int *b03 = b0.Qstate(3); // this is for blocked state 0 priority 3
int *b10 = b1.Qstate(0); // this is for blocked state 1 priority 0
int *b11 = b1.Qstate(1); // this is for blocked state 1 priority 1
int *b12 = b1.Qstate(2); // this is for blocked state 1 priority 2
int *b13 = b1.Qstate(3); // this is for blocked state 1 priority 3
int *b20 = b2.Qstate(0); // this is for blocked state 2 priority 0
int *b21 = b2.Qstate(1); // this is for blocked state 2 priority 1
int *b22 = b2.Qstate(2); // this is for blocked state 2 priority 2
int *b23 = b2.Qstate(3); // this is for blocked state 2 priority 3
int *r0 = ready.Qstate(0);
int *r1 = ready.Qstate(1);
int *r2 = ready.Qstate(2);
int *r3 = ready.Qstate(3);

cout <<"*****************************************************\n";
cout<<"The current system state is as follows:\n";
cout<<"*****************************************************\n\n";

cout<<"CURRENT TIME: "<<time<<endl<<endl;

cout<<"RUNNING PROCESS:"<<endl;
cout<<"PID  Priority Value  Start Time  Total CPU time"<<endl;

cout<<" "<<cpu[CPU]<<"    "<<pcb[cpu[CPU]].priority<<"         "<<pcb[cpu[CPU]].value<<"        "<<pcb[cpu[CPU]].start_time<<"            "<<pcb[cpu[CPU]].cpu_time<<endl<<endl;

cout<<"BLOCKED PROCESS: "<<endl;
if(b0.QAsize()==0){
cout<<"Queue of processes Blocked for resource 0 is empty"<<endl;
}else

{

cout<<"Queue of processes Blocked for resource 0:"<<endl;
cout<<"PID  Priority Value  Start Time  Total CPU time"<<endl;
if(b0.Qsize(0)==0){
//do nothing
}else{
	
	for(int j=0; j<b0.Qsize(0); j++){
	int tempPid = b00[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}

if(b0.Qsize(1)==0){
//do nothing
}else{

	for(int j=0; j<b0.Qsize(1); j++){
	int tempPid = b01[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}
if(b0.Qsize(2)==0){
//do nothing
}else{

	for(int j=0; j<b0.Qsize(2); j++){
	int tempPid = b02[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}
if(b0.Qsize(3)==0){
//do nothing
}else{

	for(int j=0; j<b0.Qsize(3); j++){
	int tempPid = b03[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}
	
}

//blocked for r1
if(b1.QAsize()==0){
cout<<"Queue of processes Blocked for resource 1 is empty"<<endl;
}else

{

cout<<"Queue of processes Blocked for resource 1:"<<endl;
cout<<"PID  Priority Value  Start Time  Total CPU time"<<endl;
if(b1.Qsize(0)==0){
//do nothing 
}else{
	
	for(int j=0; j<b1.Qsize(0); j++){
	int tempPid = b10[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}

if(b1.Qsize(1)==0){
//do nothing
}else{

	for(int j=0; j<b1.Qsize(1); j++){
	int tempPid = b11[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}
if(b1.Qsize(2)==0){
//do nothing
}else{

	for(int j=0; j<b1.Qsize(2); j++){
	int tempPid = b12[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}
if(b1.Qsize(3)==0){
//do nothing
}else{

	for(int j=0; j<b1.Qsize(3); j++){
	int tempPid = b13[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}
	
}	

//blocked for r2
if(b2.QAsize()==0){
cout<<"Queue of processes Blocked for resource 2 is empty"<<endl<<endl;
}else

{

cout<<"Queue of processes Blocked for resource 2:"<<endl;
cout<<"PID  Priority Value  Start Time  Total CPU time"<<endl;
if(b2.Qsize(0)==0){
//do nothing
}else{
	
	for(int j=0; j<b2.Qsize(0); j++){
	int tempPid = b20[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}

if(b2.Qsize(1)==0){
//do nothing
}else{

	for(int j=0; j<b2.Qsize(1); j++){
	int tempPid = b21[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}
if(b2.Qsize(2)==0){
//do nothing
}else{

	for(int j=0; j<b2.Qsize(2); j++){
	int tempPid = b22[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}
if(b2.Qsize(3)==0){
//do nothing
}else{

	for(int j=0; j<b2.Qsize(3); j++){
	int tempPid = b23[j];
	cout<<" "<<tempPid<<"    "<<pcb[tempPid].priority<<"         "<<pcb[tempPid].value<<"        "<<pcb[tempPid].start_time<<"            "<<pcb[tempPid].cpu_time<<endl;
	
	}
}
	
}



cout <<"PROCESSES READY TO EXECUTE:"<<endl;
if(ready.Qsize(0)==0){
cout<<"Queue of processes with priority 0 is empty"<<endl;
}else{
cout<<"Queue of processes with priority 0:"<<endl;
cout<<"PID  Priority Value  Start Time  Total CPU time"<<endl;
	for(int j=0; j<ready.Qsize(0); j++){
	cout<<" "<<r0[j]<<"    "<<pcb[r0[j]].priority<<"         "<<pcb[r0[j]].value<<"        "<<pcb[r0[j]].start_time<<"            "<<pcb[r0[j]].cpu_time<<endl;
	
	}
}
if(ready.Qsize(1)==0){
cout<<"Queue of processes with priority 1 is empty"<<endl;
}else{
cout<<"Queue of processes with priority 1:"<<endl;
cout<<"PID  Priority Value  Start Time  Total CPU time"<<endl;
	for(int j=0; j<ready.Qsize(1); j++){
	cout<<" "<<r1[j]<<"    "<<pcb[r1[j]].priority<<"         "<<pcb[r1[j]].value<<"        "<<pcb[r1[j]].start_time<<"            "<<pcb[r1[j]].cpu_time<<endl;
	
	}
}
if(ready.Qsize(2)==0){
cout<<"Queue of processes with priority 2 is empty"<<endl;
}else{
cout<<"Queue of processes with priority 2:"<<endl;
cout<<"PID  Priority Value  Start Time  Total CPU time"<<endl;
	for(int j=0; j<ready.Qsize(2); j++){
	cout<<" "<<r2[j]<<"    "<<pcb[r2[j]].priority<<"         "<<pcb[r2[j]].value<<"        "<<pcb[r2[j]].start_time<<"            "<<pcb[r2[j]].cpu_time<<endl;
	
	}
}
if(ready.Qsize(3)==0){
cout<<"Queue of processes with priority 3 is empty"<<endl;
}else{
cout<<"Queue of processes with priority 3:"<<endl;
cout<<"PID  Priority Value  Start Time  Total CPU time"<<endl;
	
	for(int j=0; j<ready.Qsize(3); j++){
	int tempPid = r3[j];
	cout << " " <<tempPid << "    " << pcb[tempPid].priority <<"         " << pcb[tempPid].value << "        " << pcb[tempPid].start_time << "            " << pcb[tempPid].cpu_time << endl;
	
	}
}

cout << "*****************************************************\n" << endl;


}
void startProcess(vector<int> &cpu, QueueArray<int> &ready, vector<pcb> &pcb){
	// make sure quantum is up before start
	
	cpu[CPU] = ready.Dequeue();
	cpu[QUANTUM] = pcb[cpu[CPU]].quantum;
	
	
}

void block(int rid, vector<int> running, vector<pcb> &pcb,QueueArray<int> &b0,QueueArray<int> &b1,QueueArray<int> &b2 ){

    int oldPrior = pcb[running[CPU]].priority;
	if (oldPrior >0){
	oldPrior--;
	pcb[running[CPU]].setPriority(oldPrior);
	}
		int temp;
	switch(rid){
	
	case 0: temp= b0.Enqueue(running[CPU],oldPrior);
	break;

	case 1: temp= b1.Enqueue(running[CPU],oldPrior);
	break;	

	case 2: temp= b2.Enqueue(running[CPU],oldPrior);
	break;
	
	default: cout<<"error in blocking\n";
	break;

	}
}
