#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
 
/* processManager.cpp file */

int main(int argc, char *argv[]) {
  int i, max = 100;
  int mcpipe2[2], num;
  char chr;
  int pid,rid,value,run_time;
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
      cout <<"\nchild read pid: \n"<<pid<<endl;

	//cmd was s read value
      read(mcpipe2[0], &value, sizeof(int));
      cout <<"\nchild read value: \n"<<value<<endl;

	//cmd was s read run_time
      read(mcpipe2[0], &run_time, sizeof(int));
      cout <<"\nchild read run_time: \n"<<run_time<<endl;
    }
    else if(chr== 'B'){
	
	//cmd was B read rid
      read(mcpipe2[0], &rid, sizeof(int));
      cout <<"\nchild read rid: \n"<<rid<<endl;
    }
    else if(chr== 'U'){

	
	//cmd was U read rid
      read(mcpipe2[0], &rid, sizeof(int));
      cout <<"\nchild read rid: \n"<<rid<<endl;
	
    }
    else if(chr== 'Q'){
	//end of one time unit 
	// do something with time here 
    }
    else if(chr== 'C'){
	//cmd was C read cmd and num
	  read(mcpipe2[0], &cmd, sizeof(char));
      cout <<"\nchild read cmd: \n"<<cmd<<endl;

	//cmd was C read num
      read(mcpipe2[0], &num, sizeof(int));
      cout <<"\nchild read num: \n"<<num<<endl;
    }
    else if(chr== 'P'){
	// doesn't need a command, it should be handled by process manager, print state
	// this should trigger the fork, needs to print state
    }
  
    else{
	std::cout<<"something happens in my input, error from else in proc mang\n";
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

