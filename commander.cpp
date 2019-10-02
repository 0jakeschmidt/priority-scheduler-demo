#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "pcb.h"
#include "queue_array.h" 
using namespace std;

/* master.cpp file */
#define READ_END 0
#define WRITE_END 1

int main()
 
{
  int max = 100, i, c1, stat;
  ifstream fd;  

  //pipe variable
  int mcpipe1[2], status;
  //to pass the pipe address to the child process
  char mc0[10], mc1[10],chr ;
 
  //create the pipe
  if (c1 = pipe(mcpipe1)) /* Create a pipe for master and a child process */ {
    perror("unable to create the pipe");
    exit(1);
  }
  
  //convert the addresses into a character so it can be passed as parameter to the child processes 
  sprintf(mc0, "%d", mcpipe1[READ_END]);
  sprintf(mc1, "%d", mcpipe1[WRITE_END]);

  //fork the child process
  if ((c1 = fork()) == -1) /* Create the first child process */ {
    perror("unable to fork child");
    exit(1);
  }
  else if (c1 == 0) {
    /* This is the first child process */
    cout <<"Child pid = "<<getpid()<<endl;

    //exec and pass the parameters to the child
    execl("processManager", "processManager", mc0, mc1, NULL);
    std::cout<<"execl failed in cmdr\n";
    exit(1);
  }
  //this code is what the master now executes
  cout <<"master pid = "<<getpid()<<endl;

  //close the read side of the pipe, since we don't read from the child
  close(mcpipe1[READ_END]);  

  //read in a single character
  cin >>chr;
    while (chr!='T') {
    
   
	 write(mcpipe1[WRITE_END], (char *)&chr, sizeof(char));
    if(chr == 'S'){
	cin >>i;
    
      //write to the child process the PID value 
      write(mcpipe1[WRITE_END], (int *)&i, sizeof(int));
	
	cin >>i;
     
      //write to the child process the value 
      write(mcpipe1[WRITE_END], (int *)&i, sizeof(int));

	cin >>i;
    
      //write to the child process the run_time
      write(mcpipe1[WRITE_END], (int *)&i, sizeof(int));
    }
    else if(chr== 'B'){
	
      cin >>i;
   
      //write to the child process the RID
      write(mcpipe1[WRITE_END], (int *)&i, sizeof(int));
    }
    else if(chr== 'U'){

      cin >>i;
     // cout <<"Master Writing "<<i<<endl;
      //write to the child process the RID
      write(mcpipe1[WRITE_END], (int *)&i, sizeof(int));
	
    }
    else if(chr== 'Q'){
	//do nothing here, process handled by process manager
    }
    else if(chr== 'C'){
	 cin >>chr;
         //cout<<"Master: writing "<<chr<<endl;
	//write cmd val to the child process
	 write(mcpipe1[WRITE_END], (char *)&chr, sizeof(char));

      cin >>i;
    //  cout <<"Master Writing "<<i<<endl;
      //write to the child process the num
      write(mcpipe1[WRITE_END], (int *)&i, sizeof(int));
    }
    else if(chr== 'P'){
	// doesn't need a command, it should be handled by process manager, print state
    }
  
    else{
	// this is an invalid command, ignore
	
    }

    cin >>chr; //Reading next line

    //sleep for 1 second
    sleep (1);
	}
  //write out the last one, which is a T, so the wrt process receives the P and exits
  write(mcpipe1[WRITE_END], (char *)&chr, sizeof(char));
  cout <<"finished reading commands\n";

  //close the write side of the pipe
  close(mcpipe1[WRITE_END]);
  
  //wait for the child to end
  wait(&status); //and get the status of the child.
  //note with exit and return it's the value & 0377,and WEXITSTATUS will tell us the "real" return value
  cout << "Child status is "<<WEXITSTATUS(status)<<endl;
  exit(0);

 
}
