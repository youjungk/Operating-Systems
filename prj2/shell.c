#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

/*global variables*/
#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10
//For the history feature in internal command handle
static int num_of_commands =0;
char historyCommand[HISTORY_DEPTH][COMMAND_LENGTH];

//implicit function declaration
int execute_history_command(char* buff, char* tokens[], _Bool* in_background, int num) ;
void inputprompt();
int tokenize_command(char* buff, char* tokens[]);
void handle_SIGINT(int sig);
void signal_hander();
int internal_command_helper(char* buff, char* tokens[], _Bool* in_background);
int execute_history_command(char* buff, char* tokens[], _Bool* in_background, int num) ;
void read_command(char *buff, char *tokens[], _Bool *in_background);
void print_history();
/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 * COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 * least NUM_TOKENS long. Will strip out up to one final '&' token.
 * 'tokens' will be NULL terminated.
 * in_background: pointer to a boolean variable. Set to true if user entered
 * an & as their last token; otherwise set to false.
*/

void inputprompt()
{
	char curr_dir[COMMAND_LENGTH];
	//get current working directory: type int
	if (getcwd(curr_dir, COMMAND_LENGTH) != NULL) {
		write(STDOUT_FILENO, curr_dir, strlen(curr_dir));
	}
	else
	{
		write(STDOUT_FILENO, "Unable to get current directory\n", strlen("Unable to get current directory\n"));
	}
	write(STDOUT_FILENO, "> ", strlen("> "));
}

void print_history(){
	char cmdNum[100];
	if (num_of_commands > HISTORY_DEPTH) {
		for (int i = 0; i < HISTORY_DEPTH; i++) {
			sprintf(cmdNum, "%d", (num_of_commands - HISTORY_DEPTH + i + 1));
			write(STDOUT_FILENO, cmdNum, strlen(cmdNum));
			write(STDOUT_FILENO, "\t", strlen("\t"));
			write(STDOUT_FILENO, historyCommand[(num_of_commands + i) % HISTORY_DEPTH], strlen(historyCommand[(num_of_commands + i) % HISTORY_DEPTH]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
	}
	else {
		for (int j = 0; j < num_of_commands; j++) {
			sprintf(cmdNum, "%d", (1 + j));
			write(STDOUT_FILENO, cmdNum, strlen(cmdNum));
			write(STDOUT_FILENO, "\t", strlen("\t"));
			write(STDOUT_FILENO, historyCommand[j], strlen(historyCommand[j]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
	}
}
int tokenize_command(char* buff, char* tokens[])
{
	if (buff == NULL) {
		return 0;
	}

	int num_Tokens = 0;
	char* curr_Token;

	curr_Token = strtok(buff, " ");
	if (curr_Token != NULL) {
		tokens[num_Tokens] = curr_Token;
		num_Tokens++;
	}

	while (curr_Token != NULL) {
		curr_Token = strtok(NULL, " ");
		if (curr_Token != NULL) {
			tokens[num_Tokens] = curr_Token;
			num_Tokens++;
		}
	}

	tokens[num_Tokens] = NULL;
	return num_Tokens;
}

void handle_SIGINT(int sig)
{
	write(STDOUT_FILENO, "\n", strlen("\n"));
	inputprompt();
}

void signal_hander()
{
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);
}

int internal_command_helper(char* buff, char* tokens[], _Bool* in_background){
	//exit: exit the program
	//pwd: display the current working directory
	//cd: change directory
	//history: displays history feature
	//!n: runs command number n
	//!!: execute last command in history
	
	if (strcmp(tokens[0], "exit") == 0) { 
		return -1;
	}

	else if (strcmp(tokens[0], "pwd") == 0) {
		//copy the command into the array and increment the global variable "num_of_commands"
		strcpy(historyCommand[(num_of_commands%HISTORY_DEPTH)], tokens[0]);
		num_of_commands++;
		//write current directory
			//printf("number is: %d\n",num_of_commands);
		char curr_directory[COMMAND_LENGTH];
		if (getcwd(curr_directory, sizeof(curr_directory)) != NULL) {
			write(STDOUT_FILENO, curr_directory, strlen(curr_directory));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
		else {
			write(STDERR_FILENO, "Error: could not display any directory.\n", strlen("Error: could not display any directory.\n"));
		}
		return 1;
	}

	else if (strcmp(tokens[0], "cd") == 0) {
		//If chdir returns an error display an error message
		if (chdir(tokens[1]) != 0) {
			write(STDOUT_FILENO, "Error:non-existing directory\n", strlen("Error:non-existing directory\n"));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
		if (tokens[1] != NULL) {
				strcpy(historyCommand[num_of_commands%HISTORY_DEPTH], tokens[0]);
				strcat(historyCommand[num_of_commands%HISTORY_DEPTH], " ");
				strcat(historyCommand[num_of_commands%HISTORY_DEPTH], tokens[1]);
				num_of_commands++;
		}	
		return 1;
	}
		
	

	
	else if (strcmp(tokens[0], "history") == 0) {
		strcpy(historyCommand[num_of_commands%HISTORY_DEPTH], tokens[0]);
		num_of_commands++;
		print_history();
		return 1;
	}

	else if(tokens[0][0]== '!'){
		if (strlen(tokens[0]) > 1) {
			if (tokens[0][1] == '!'){
				return execute_history_command(buff, tokens, in_background, num_of_commands);
			}
			int num = atoi(&(tokens[0][1]));
			return execute_history_command(buff, tokens, in_background, num);
		}
	}

	strcpy(historyCommand[num_of_commands%HISTORY_DEPTH], tokens[0]);
	int counter = 0;
	// for (int i=0;tokens[i] != NULL;i++){
	while (tokens[counter+1]!= NULL){
		strcat(historyCommand[num_of_commands%HISTORY_DEPTH], " ");
		strcat(historyCommand[num_of_commands%HISTORY_DEPTH], tokens[counter+1]);
		counter++;
	}
	 if (*in_background){
	 	strcat(historyCommand[num_of_commands% HISTORY_DEPTH], " /finish");
	}
	num_of_commands++;
	return 0;
}

int execute_history_command(char* buff, char* tokens[], _Bool* in_background, int num) 
{
	//printf("number is3: %d\n",num);
	if (num < 1 || num >num_of_commands || num <(num_of_commands - 9)) {
		write(STDERR_FILENO, "the command is out of range. \n", strlen("the command is out of range. \n"));
		return 1;
	}

	//get the nth command from history
	strcpy(buff, historyCommand[(num - 1) % HISTORY_DEPTH]);
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		write(STDERR_FILENO, "Error: unable to get command from history.", strlen("Error: unable to get command from history."));
		return 1;
	}
	*in_background = false;
	if (strcmp(tokens[token_count - 1], "/finish") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}

	write(STDOUT_FILENO, historyCommand[(num - 1) % HISTORY_DEPTH], strlen(historyCommand[(num - 1) % HISTORY_DEPTH]));
	write(STDOUT_FILENO, "\n", strlen("\n"));
	return internal_command_helper(buff, tokens, in_background);
}


 /**
 * Main and Execute Commands
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;
	// Read input
	int length;
	do {
		length = read(STDIN_FILENO, buff, COMMAND_LENGTH - 1);
	} while (length < 0 && errno == EINTR);

	if ((length < 0) && (errno != EINTR))
	{
		perror("Unable to read command. Terminating.");
		exit(-1); /* terminate with error */
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}


	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
}
int main(int argc, char* argv[])
{
	/*Steps for basic shell*/
	//1. Fork a child process
	//2. Child process invokes execvp() using results in token array.
	//3. If in_backgrround is false. parent waits until child to finish otherwise read_command executes
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	//Signal handling function call
	signal_hander();
	while (true) {
		//calling input prompt
		inputprompt();

		_Bool in_background = false;
		read_command(input_buffer, tokens, &in_background);

		//check after read_command if 'input_buffer' and 'tokens' is empty
		if (input_buffer[0] == '\0' || tokens[0] == NULL) {
			continue;
		}
		//internal commands: for exit,pwd,cd
		int internal_command = internal_command_helper(input_buffer, tokens, &in_background);

		//exit
		if (internal_command == -1) {
			return 0;
		}
		else if (internal_command == 1) {
			continue;
		}

		//child process (fork)
		pid_t pid= fork();

		if (pid < 0) {
			write(STDERR_FILENO, "Error: fork failed\n", strlen("Error: Fork failed\n"));
		}
		if (pid == 0) {
			if (execvp(tokens[0], tokens) == -1) {
				write(STDERR_FILENO, "Error in executing the program\n", strlen("Error in executing the program\n"));
				exit(-1);
			}
			exit(0);
		}
		if (in_background == false) {
			while (waitpid(-1, NULL, WNOHANG) > 0);
		}
		else {
			continue;
		}
	}
	return 0;
}


