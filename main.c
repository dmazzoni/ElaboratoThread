/** @file
	Code for the main thread, which performs 
	simulation setup and execution management.<br>
	After setting up the data structures, 
	the main thread does the following:<ul>
	<li>Creates the required number of processor threads
	<li>Dispatches each operation to the appropriate processor,
	collecting the latest computed result
	<li>Writes the results on the specified output file</ul>
*/

#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "io_utils.h"
#include "list.h"
#include "mutex_utils.h"
#include "project_types.h"

void* processor_routine(void *arguments);
static int find_proc(int *states, pthread_mutex_t *mutex);
static list* parse_file(const char *const pathname);
static void start_threads(pthread_t *threads, int n_threads, thread_args *args, pthread_mutex_t *mutexes, int *states, int *free_count, operation *operations, pthread_cond_t *cond);

/**
	Carries out simulation setup and management.
	@param argc The number of arguments
	@param argv The array of arguments
*/
int main(int argc, char *argv[]) {
	int *results, *states;
	int i, op_count, processor_id, n_threads;
	volatile int free_count;
	char *tmp_operator, *cmd;
	list *commands;
	operation *operations;
	pthread_cond_t cond;
	pthread_mutex_t *mutexes;
	pthread_t *threads;
	thread_args *arguments;
	
	if(argc != 3) {
		write_to_fd(2, "Usage: main.x <source file> <results file>\n");
		exit(1);
	}
	commands = parse_file(argv[1]);
	n_threads = atoi(list_extract(commands));
	if (n_threads <= 0) {
		write_to_fd(2, "Invalid number of threads\n");
		exit(1);
	}
	free_count = n_threads;
	write_with_int(1, "Number of threads: ", n_threads);
	op_count = list_count(commands);
	if (op_count == 0) {
		write_to_fd(2, "No operations provided\n");
		exit(1);
	}
	write_with_int(1, "Number of operations: ", op_count);		
	
	if(pthread_cond_init(&cond, NULL) != 0) {
			write_to_fd(2, "Failed to initialize condition variable\n");
			exit(1);
	}
	results = (int *) malloc(op_count * sizeof(int));
	mutexes = (pthread_mutex_t *) malloc((2 * n_threads + 1) * sizeof(pthread_mutex_t));
	threads = (pthread_t *) malloc(n_threads * sizeof(pthread_t));
	operations = (operation *) malloc(n_threads * sizeof(operation));
	states = (int *) malloc(n_threads * sizeof(int));
	arguments = (thread_args *) malloc(n_threads * sizeof(thread_args));
	if (!results || !mutexes || !threads || !operations || !states || !arguments) {
		write_to_fd(2, "Failed to allocate auxiliary data structures\n");
		exit(1);
	}
	
	init_mutexes(mutexes, 2 * n_threads + 1);
	for (i = 0; i < n_threads; ++i)
		states[i] = 0;

	start_threads(threads, n_threads, arguments, mutexes, states, &free_count, operations, &cond);
	for (i = 1; list_count(commands) > 0; ++i) {
		cmd = list_extract(commands);
		write_with_int(1, "\nOperation #", i);
		processor_id = atoi(strtok(cmd, " "));
		lock(&mutexes[2 * n_threads]);
		write_with_int(1, "Sono QUI", free_count);
		while (free_count == 0)
			if(pthread_cond_wait(&cond, &mutexes[2 * n_threads]) != 0)
				write_to_fd(2, "Failed to wait on condition\n");
		--free_count;
		unlock(&mutexes[2 * n_threads]);
		if (processor_id-- == 0) {
			processor_id = find_proc(states, &mutexes[2 * n_threads]);
		}
		write_with_int(1, "Waiting for processor ", processor_id + 1);
		lock(&mutexes[2 * processor_id]);
		unlock(&mutexes[2 * processor_id + 1]);
		write_with_int(1, "Delivering operation to processor ", processor_id + 1);
		if (states[processor_id] != 0) {
			results[(states[processor_id] + 1) * -1] = operations[processor_id].num1;
			write_with_int(1, "Previous result: ", operations[processor_id].num1);
		}
		operations[processor_id].num1 = atoi(strtok(NULL, " "));
		tmp_operator = strtok(NULL, " ");
		operations[processor_id].op = *tmp_operator;
		operations[processor_id].num2 = atoi(strtok(NULL, " "));
		states[processor_id] = i;
		write_with_int(1, "Operation delivered. Unblocking processor ", processor_id + 1);
		lock(&mutexes[2 * processor_id + 1]);
		unlock(&mutexes[2 * processor_id]);
		/**if (pthread_yield() != 0) {
			write_to_fd(2, "Failed to yield\n");
			exit(1);
		}*/
		pthread_sleep(1);
		free(cmd);
	}
	
	list_destruct(commands);
	
	for (i = 0; i < n_threads; ++i) {
		lock(&mutexes[2 * processor_id]);
		unlock(&mutexes[2 * processor_id + 1]);
		write_with_int(1, "\nPassing termination command to processor #", i + 1);
		if (states[i] != 0) {
			results[(states[i] + 1) * -1] = operations[i].num1;
			write_with_int(1, "Last result: ", operations[i].num1);
		}
		operations[i].op = 'K';
		unlock(&mutexes[2 * processor_id]);
	}

	for (i = 0; i < n_threads; ++i) {
		if (pthread_join(threads[i], NULL) != 0)
			write_with_int(2, "Failed to join thread ", i + 1);
		destroy(&mutexes[2 * i]);
		destroy(&mutexes[2 * i + 1]);
	}
	destroy(&mutexes[2 * n_threads]);
			
	free(mutexes);
	free(threads);
	free(arguments);
	free(operations);
	free(states);		
	write_to_fd(1, "\nAll threads exited. Writing output file\n");
	write_results(argv[2], results, op_count);
	free(results);
	exit(0);
}

/**
	Searches the processor state array for a free processor (state <= 0).
	@param states The array of processor states. Each cell <i>i</i> contains a 
	key such that:<br>
	<ul><li>If <b>key < 0</b>, processor has completed the |key|-th operation
	<li>If <b>key == 0</b>, processor has not received an operation yet
	<li>If <b>key > 0</b>, processor is currently working on the key-th operation</ul>
	@param mutex The mutex which protects the states array
	@return The ID of a free processor
*/
static int find_proc(int *states, pthread_mutex_t *mutex) {
	int i = 0;

	lock(mutex);
	write_to_fd(1, "Looking for a free processor\n");
	while(states[i++] > 0);
	unlock(mutex);
	write_with_int(1, "Found processor ", i);
	return i - 1;
}

/**
	Reads the specified setup file, 
	building a list of strings containing the operations to simulate.<br>
	Assumes that the file has a correct structure.
	@param pathname The setup file's path
	@return The list of operations to compute
*/
static list* parse_file(const char *const pathname) {
	list *result = list_construct();
	char line[50];
	int len, fd;
	
	if(result == NULL) 
		exit(1);

	fd = open(pathname, O_RDONLY);
	if(fd == -1) {
		write_to_fd(2, "Failed to open setup file\n");
		exit(1);
	}
		
	do {
		len = read_line(fd, line, 50);
		if (len > 0)
			list_append(result, line);
	} while(len >= 0);
	
	if (close(fd) == -1) {
		write_to_fd(2, "Failed to close setup file\n");
		exit(1);
	}

	return result;
}

/**
	Creates the required number of threads.
	//TODO
*/
static void start_threads(pthread_t *threads, int n_threads, thread_args *args, pthread_mutex_t *mutexes, int *states, int *free_count, operation *operations, pthread_cond_t *cond) {
	int i;
	
	for (i = 0; i < n_threads; ++i) {
		args[i].processor_id = i;
		args[i].mutexA = &mutexes[2 * i];
		args[i].mutexB = &mutexes[2 * i + 1];
		args[i].cond_mutex = &mutexes[2 * n_threads];
		args[i].oper = &operations[i];
		args[i].state = &states[i];
		args[i].free_count = free_count;
		args[i].cond = cond;
		if (pthread_create(&threads[i], NULL, processor_routine, (void *) &args[i]) != 0) {
			write_with_int(2, "Failed to create thread ", i + 1);
		}
	}
}
