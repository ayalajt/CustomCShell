////////////////////////////////////////////////////////////////////////////////
//
//  File           : shellex.c
//  Description    : This is the source code for a Shell implementation
//
//   Author        : Jesus Ayala
//   Last Modified : December 6th, 2019
//

// $begin shellmain 
#include "csapp.h"
#define MAXARGS   128

// Function prototypes 
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 

void sigHandler(int sig) { // This function makes it so ctrl+C does not exit shell
	signal(SIGINT, sigHandler);
	fflush(stdout);
}

int main(int argc, char** argv) {
	signal(SIGINT, sigHandler);
	char cmdline[MAXLINE]; // Command line
	while (1) {
		if (argc == 3 && (strcmp(argv[1],"-p")== 0)) {
			printf("%s>", argv[2]); // Check if there is a valid prompt
		}
		else {
			printf("my257sh>"); // If not, then default to this string
		}                
		Fgets(cmdline, MAXLINE, stdin); 
		if (feof(stdin)) {
			exit(0);
		}
		eval(cmdline); //Evaluate
	} 
}

// $end shellmain 
// $begin eval 
// eval - Evaluate a command line 
void eval(char *cmdline) {
	char *argv[MAXARGS]; // Argument list execve()
	char buf[MAXLINE];   // Holds modified command line 
	int bg;              // Should the job run in bg or fg? 
	pid_t pid;           // Process id

	strcpy(buf, cmdline);
	bg = parseline(buf, argv); 
	if (argv[0] == NULL)  
		return;   // Ignore empty lines 

	if (!builtin_command(argv)) { 
		if ((pid = Fork()) == 0) {   // Child runs user job 
			if (execvp(argv[0], argv) < 0) {
				printf("Execution failed (in fork)\n");
				printf(": No such file or directory\n");
				exit(1); // the child status is 1 for invalid commands
			}
		}
		// Parent waits for foreground job to terminate
		if (!bg) {
			int status;
			if (waitpid(pid, &status, 0) < 0) {
				unix_error("waitfg: waitpid error");
			}
			waitpid(pid, &status, 0);
			if (WIFEXITED(status)) {
				int exit_status = WEXITSTATUS(status);
				printf("Child exit status: %d\n", exit_status);
				// Grab status of the child
			}
		}
		else
			printf("%d %s", pid, cmdline);
		}
		return;
}
// If first arg is a builtin command, run it and return true
int builtin_command(char **argv) {
	if (!strcmp(argv[0], "exit")) { // quit command
		printf("Terminated\n");
			exit(0);  
	}	
	if (!strcmp(argv[0], "&")) {   // Ignore singleton %
		return 1;
	}
	if (!strcmp(argv[0], "pid")) { // Grab process id
		printf("%d\n", getpid());
		return 1;
	}
	if (!strcmp(argv[0], "ppid")) {	// Grab parent process id
		printf("%d\n", getppid());
		return 1;
	}
	if (!strcmp(argv[0], "cd")) {
		if (argv[1] == NULL) { // If no option inputted after cd, then printcurrent directory
			char str[1000];	
			printf("%s\n", getcwd(str, 1000));
			return 1;
		}
		else { // Otherwise go to the option inputted
			chdir(argv[1]);
			return 1;
		}
	}
	if (!strcmp(argv[0], "help")) { // Help command prints useful info
		printf("Shell created by Jesus Ayala\n");
		printf("Useful commands:\n");
		printf("exit: Exits the shell\n");
		printf("pid: prints the process id of the shell\n");
		printf("ppid: prints the parent process id of the shell\n");
		printf("cd: print current working directory\n");
		printf("cd <path>: change current working directory\n");
		printf("Refer to man for other commands\n");
		return 1;
	}  
	return 0; // Only reach here if not a built in command
}

// $end eval
// $begin parseline 
// parseline - Parse the command line and build the argv array 
int parseline(char *buf, char **argv) {
	char *delim; // Points to first space delimiter
	int argc; // Number of args 
	int bg; // Background job? 
	buf[strlen(buf)-1] = ' ';  // Replace trailing '\n' with space 
	while (*buf && (*buf == ' ')) // Ignore leading spaces 
		buf++;

	// Build the argv list
	argc = 0;
	while ((delim = strchr(buf, ' '))) {
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while (*buf && (*buf == ' ')) // Ignore spaces
			buf++;
	}
	argv[argc] = NULL;
	if (argc == 0)  // Ignore blank line 
		return 1;
	
	// Should the job run in the background? 
	if ((bg = (*argv[argc-1] == '&')) != 0)
		argv[--argc] = NULL;

		return bg;
}
// $end parseline 
