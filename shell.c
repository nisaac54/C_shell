#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/resource.h>
#include "queue.h"
#include <signal.h>
#include <wait.h>



int fg_pid = 0;
int fg_suspended = 0;
int run = 1;
struct queue pid_list;

char *args[10];
int backround = 1;

void help() {
	printf("This is manual page\n");
	printf("This shell supports the following commands:\n");
	printf("\tver\n\texec\n\tps\n\tkill\thelp\n\texit\n\tclear\n");
	printf("For more details please type 'help command'\n");
}

void helpcmd(char *cmd) {
	printf("This is manual page\n\n");
	if (strcmp(cmd,"ver")==0)
	{
		printf("\nver:\tShows details about the shell version\n");
	}
	else if (strcmp(cmd,"exec")==0) 
	{
		printf("\nexec p1(n1,qt1) p2(n2,qt2) ...:\nExecutes the programs p1, p2 ...\nEach program types a message for n times and it is given a time quantum of qt msec.\n");
		printf("If parameter (&) is given the program will be executed in the background\n");
	}
	else if (strcmp(cmd,"ps")==0)
	{
		printf("\nps:\tShows the living process with the given pid\n");
	}
	else if (strcmp(cmd,"kill")==0)
	{
		printf("\nkill pid:\tEnds the process with the given pid\n");
	}
	else if (strcmp(cmd,"help")==0)
	{
		printf("\nhelp:\tYou should know this command by now\n");
	}
	else if (strcmp(cmd,"exit")==0)
	{
		printf("\nexit:\tEnds the experience of working in the new shell\n");
	}
	else if (strcmp(cmd,"clear")==0)
	{
		printf("\nclear the screen\n");
	}
	else 
	{
		printf("\nNo Such command. Type help to see a list of commands\n");
	}
}

void clean(){ 

	//clear the screen
	const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  	write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
	
} 

void ver() {
	printf("\nNew Shell. Works properly!!\n");
}

void deallocate_memory_function(char **args, const int N){

	printf("Freed: %d\n",pid_list.head->pid);
		int i;
	 	for (i = 0; i < N; i++) {
			free(args[i]); 
			++args; //increment to the next pointer to remove
    	}

}

void ps() {
	struct node *p;
	printf("\n");
	printf("\nNEW SHELL presents the following living processes\n");
	printf("\tPID\tNAME\n");
	
	if(pid_list.head == NULL){
		pid_list.tail = NULL;
		printf("\tList is empty\n");
		return;
	}
	else{

		for(p = pid_list.head; p != NULL; p = p->next){
			printf("\t%d\t%s\n", p->pid, p->name);
		}
		printf("\n");
	}
}

void mykill(int pid){

	// check if head is the element to remove
	if(pid_list.head->pid == pid){
		deallocate_memory_function(args, sizeof(args) / sizeof(char *) ); // used to not hard code //(args,10)
		pid_list.head = pid_list.head->next;

	}
	else{
		kill(pid,SIGTERM);
	}
	printf("\n");

	printf("You have just killed process %d\n",pid );
}


void *exec(char *input) {
	int i,t,status;
	char *args[10];
	char *temp;
	struct node *p;
	backround = 1;

	// Execute a processes

	for(i=0; i<10; i++){
		args[i] = (char *)malloc(10*(sizeof(char))); //allocate the memory of args on heap
	}

	strcpy(args[0],strtok(input,"(,")); // token 1 = (
	for(i=1;(temp = strtok(NULL,",)")) != NULL; i++){ // token 2 = )
		strcpy(args[i],temp);

	}
	if (strcmp(args[i-1],"&")==0){
		args[i-1] = NULL;
		backround = 0;
		printf("\n");
	}
	else{
		args[i] = NULL;
		backround = 1;
	}

	if((t=fork())==0){
		if(backround == 0){ // Execute in backround   
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			execv(args[0],args);
		}
		else{ // Execute in forground
			execv(args[0],args);
		}
	}

	enqueue(t, args[0], &pid_list);

	//If in forground deallocate memory 
	if(backround == 1){
		int j;
		for(j=0;j<10;j++){
			free(args[j]);
		}
		printf("\n");
	}

	// Allows one forground process to be stored in queue at a time  
	// Return to shell and wait for command after a processe(s) have finished
	if(args[i-1] != NULL){
		fg_pid = t;
		while(fg_pid != 0 && fg_suspended != 1){
			pause();
		}			
	}	
}


void myexit() {

	char user_input;

	//if list is empty exit the program
	if(pid_list.head == pid_list.tail){
		run = 0;
	}
	else{ //list is not empty
		printf("Some processes are still alive...\n");
		printf("Do you want to kill them (y/n) ");

		user_input = getchar(); //allow user to kill living processes
		printf("\n");
		if (user_input == 'y'){ 
			while (pid_list.head != pid_list.tail){
				mykill(pid_list.tail->pid);
				usleep(100000);
			}
			run = 0;
		}
		if (user_input == 'n'){
			run = 0;
		}
	}
}

void childdead(int signum){
	int dead_pid, status;
	dead_pid=wait(&status);

	printf("The child %d is dead\n",dead_pid);

	//Deallocate memory using delete function 
	delete(&pid_list,dead_pid);
	printf("\n");

	//Keep the shell in the forground
	if (dead_pid == fg_pid)
	{
		fg_pid = 0;
	}
}


void susp (int signum) {
	fg_suspended = 1;
	printf("All processes supspended\n");
}


void cont (int signum) {
	fg_suspended = 0;
	printf("Waking all processes...\n");
	while (fg_pid != 0 && fg_suspended != 1)
		pause();
}


int main(int argc, char const *argv[]) {
	char input[15][30];
	int argnum, i;

	pid_list.head = NULL;
	pid_list.tail = NULL;
	enqueue(getppid(),"NEW SHELL",&pid_list);

	signal(SIGCHLD,childdead);
	signal(SIGTSTP,susp);
	signal(SIGQUIT,cont);
	setpriority(PRIO_PROCESS,0,-20);

	ver();

	while (run){
		printf("=>");
		for (argnum=0; (scanf("%s",&input[argnum]))==1;argnum++)
			if (getchar()=='\n') break;
		if (strcmp(input[0],"ver")==0 && argnum==0) ver();
		else if (strcmp(input[0],"help")==0 && argnum==0) help();
		//else if (strcmp(input[0],"help")==0 && argnum==1) helpcmd(input[argnum]);
		else if (strcmp(input[0],"ps")==0 && argnum==0) ps();
		else if (strcmp(input[0],"kill")==0 && argnum==1) mykill(atoi(input[1]));
		else if (strcmp(input[0],"clear")==0 && argnum ==0) clean();
		else if (strcmp(input[0],"exec")==0 && argnum!=0)
			for (i=1; i<=argnum; i++) exec(input[i]);
		else if (strcmp(input[0],"exit")==0 && argnum==0) myexit();
	    else printf("No such command. Check help for help.\n");
	}

}
