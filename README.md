A simple shell to execute a process
Operates on a (FCFS) first come first serve schedualer 
Supports 4 background processes ( including head )

Supports the following commands:

	- help:   shows list of commands    
	- ver:    shows details about the shell
	- kill:   kills a process with given PID
	- clear:  clear the screen
 	- ps:     shows living process with there given PID  
        - exit:   exits the program
	- exec:   p1(n1,qt1) p2(n2,qt2) ...: Executes the programs p1, p2 ... Each program types a
		  message for n times and it is given a time quantum of qt msec. If the parameter (&) is
		  given the program will be executed in the background

Compile with: 
	gcc p-shell.c -o p
	gcc shell.c -o shell
	./shell

<img width="294" alt="git_shell_output1" src="https://user-images.githubusercontent.com/62212683/144490612-45a4a9d5-4a36-45e7-b679-03b576d3ff20.PNG">
