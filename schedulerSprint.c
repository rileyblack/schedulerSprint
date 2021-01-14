#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/*
 * Process structure to hold the relevant info for each process (process number and burst time)
 */
typedef struct process{
        int process_number;
        int burst_time;
} Process;

/*
 * Queue structure to hold the relevant info for each queue (queue number and time quantum)
 */
typedef struct queue{
        int queue_number;
        int time_quantum;
} Queue;

/*
 * DESCRIPTION: Function to compute the average wait time of a queue using first come fist served algorithm
 *
 * PARAMETERS: array of struct process to emulate a queue, and an integer reflecting the size of this array (number of processes)
 *
 * RETURN: N/A
 */
void fcfs(Process array[], int size, FILE * file_ptr);

/*
 * DESCRIPTION: Function to compute the average wait time of a queue using shortest job first algorithm
 *
 * PARAMETERS: array of struct process to emulate a queue, and an integer reflecting the size of this array (number of processes)
 *
 * RETURN: N/A
 */
void sjf(Process array[], int size, FILE * file_ptr);

/*
 * DESCRIPTION: Function to compute the turnaround times of processes in a queue using round robin algorithm
 *
 * PARAMETERS: array of struct process to emulate a queue, an integer reflecting the size of this array (number of processes)
 *              and an integer reflecting the time quantum to be used in the round robin algorithm
 *
 * RETURN: N/A
 */
void rr(Process array[], int size, int time_quantum, FILE * file_ptr);

int main() {

        FILE * file_ptr = NULL; //pointer to file for reading/writing purposes
        char character; //character used to read in the file

		/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		* COUNTING QUEUES
		*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		*/

        int num_of_queues = 0; //variable to hold the total number of queues read from input file

        file_ptr = fopen("cpu_scheduling_input_file.txt", "r"); //open specified input text file for reading for queues
        if (file_ptr == NULL) { //check if file was opened without error
                printf("\n\nError opening file.\n\n"); //if an error occured, print error message and end program
                exit(0);
        }

        character = fgetc(file_ptr); //retrieve first character

        while(character != EOF){ //keep getting characters until the file is depleted (ie hit the end of file (EOF) character
                if(character == 'q'){ //if the character read is a 'q', this indicates an entry of a queue in the file
                        num_of_queues++; //increment variable holding number of queues to reflect this finding
                }
                character = fgetc(file_ptr); //get next character from input file
        }
        fclose(file_ptr); //once the file has been depleted, we should now know the number of queues in the input file

        num_of_queues = num_of_queues/2; //divided by 2 since HALF of the 'q's found were from tq (time quatum)

		/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		* COUNTING PROCESSES PER QUEUE
		*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		*/

        int *  queue_process_count = malloc(sizeof(int) * num_of_queues); //create an integer array capable of holding as many integers
                                                                          //as number of queues that were found, so that each index can
                                                                          //then be filled with the number of processes in that queue
                                                                          //for example, queue_process_count[0] will hold the integer
                                                                          //value reflecting the count of processes to be put into
                                                                          //queue 1, as per reading from the input file. Likewise,
                                                                          //queue_process_count[1] will have the number of processes
                                                                          //to be put into queue 1, etc. for each queue

        int process_counter = 0; //variable to hold the total number of processes read from current queue in question from input file
        int queue_counter = 0; //variable to keep track of which queue the processes are currently being counted for from input file

        file_ptr = fopen("cpu_scheduling_input_file.txt", "r"); //reopen specified input text file for reading, this time for processes
        if (file_ptr == NULL) { //check if the file was opened without error
                printf("\n\nError opening file.\n\n"); //if an error occured, print error message and end program
                exit(0);
        }

        character = fgetc(file_ptr); //retrieve first character

        while(character != EOF){ //keep getting characters until the file is depleted (ie hit end of file (EOF) character
                if(character == 'q'){ //check if the character read is a q (indicating a queue's data is about to start)
                        process_counter = 0; //if a q, reset the process count for this new queue
                        character = fgetc(file_ptr); //retrieve character after 'q'
                        while(character != EOF){ //keep getting characters until file has ended
                                if(character == 'q'){ //if the current character is a 'q', this means a new queue is starting so break
                                        break;
                                }
                                else if(character == 'p'){ //if the current character is a 'p' this is a process so increment counter
                                        process_counter++;
                                }
                                else if(character == 't'){ //if the current character is a 't', we want to skip the upcoming 'q' from tq
                                        character = fgetc(file_ptr); //skip over next 'q' since it comes from time quantum not a queue
                                }
                                character = fgetc(file_ptr); //if character isnt a 'q', get next to keep counting
                        }
                        queue_process_count[queue_counter] = process_counter; //reach here once an EOF or 'q' is hit indicating end of queue
                                                                               //processes, so store this count in integer array described above
                        queue_counter++; //on to the next queue in input file
                }
                else{ //if the character is not a 'q', get next character until a 'q' is hit (or EOF)
                        character = fgetc(file_ptr);
                }
        }
        fclose(file_ptr); //once the file has been depleted, we should now have the number of processes to be put in each queue

		/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		* READING/FORMATTING PROCESS/QUEUE INFORMATION INTO CREATED DATA STRUCTURES
		*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		*/

        Process ** queues = malloc(sizeof(Process *) * num_of_queues);  //create an array of process structure arrays where queues[0][0] is the first
                                                                        //process in the first queue, queues[0][1] is the second process in the first
                                                                        //queue, queues[1][0] is the first process in the second queue and so on.
        int i;
        for(i = 0; i < num_of_queues; i++){ //for each queue
                queues[i] = malloc(sizeof(Process) * queue_process_count[i]); //allocate the proper amount of memory for each queue based on number of
        }                                                                     //processes found in in that queue above (jagged array)

        int queue_num = 0; //variable to reflect the found queue number as the input file is parsed through
        int time_quantum = 0; //variable to reflect the found time quantum for the queue queue_num

        queue_counter = 0; //restart queue counter
        process_counter = 0; //restart process counter

        Queue * queues_info = malloc(sizeof(Queue) * num_of_queues); //array of queues to keep track of queue number and queue time quantums

        file_ptr = fopen("cpu_scheduling_input_file.txt", "r"); //open specified input text file for reading for queues
        if (file_ptr == NULL) { //check if file was opened without error
                printf("\n\nError opening file.\n\n"); //if an error occured, print error message and end program
                exit(0);
        }

        character = fgetc(file_ptr); //retrieve first character in file
        if(character != 'q'){ //check if retrieved character matchs expected character according to example input file format
                printf("\n\nInput file is not in the correct format, character 'q' must be the first character.\n\n");
                exit(0);
        }
        while(character == 'q'){ //continue looping while more queues to create in file
                fscanf(file_ptr, "%d", &queue_num); //retrieve queue number and store in variable
                if((queue_num < 1)||(queue_num > INT_MAX)){ //check if retrieved value is a number as expected
                        printf("\n\nInput file is not in the correct format, an integer must follow queue indicator 'q'.\n\n");
                        exit(0);
                }

                character = fgetc(file_ptr); //get next character from input file
                if(character != ' '){ //check if retrieved character matchs expected character according to example input file format
                        printf("\n\nInput file is not in the correct format, queue number and time quantum indicator 'tq' must be seperated by a space.\n\n");
                        exit(0);
                }

                character = fgetc(file_ptr); //get next character from input file
                while (character == ' '){ //while at whitespace between meaningful indicators
                        character = fgetc(file_ptr); //get next character from input file
                }

                if(character != 't'){ //check if retrieved character matchs expected character according to example input file format
                        printf("\n\nInput file is not in the correct format, time quantum indictor 'tq' must follow queue number.\n\n");
                        exit(0);
                }
                character = fgetc(file_ptr); //get next character from input file
                if(character != 'q'){ //check if retrieved character matchs expected character according to example input file format
                        printf("\n\nInput file is not in the correct format, time quantum indicator 'tq' incorrect.\n\n");
                        exit(0);
                }

                fscanf(file_ptr, "%d", &time_quantum); //retrieve time quantum number and store in variable
                if((time_quantum < 1)||(time_quantum > INT_MAX)){ //check if retrieved value is a number as expected
                        printf("\n\nInput file is not in the correct format, an integer must follow time quantum indicator 'tq'.\n\n");
                        exit(0);
                }

                Queue queue = {queue_num, time_quantum}; //create queue object with queue number and time quantum as specified in input file
                queues_info[queue_counter] = queue; //add queue to respective index in queues info array so that attributes can be later got

                character = fgetc(file_ptr); //get next character from input file
                if(character != ' '){ //check if retrieved character matchs expected character according to example input file format
                        printf("\n\nInput file is not in the correct format, time quantum number and process indicator 'p' must be seperated by a space.\n\n");
                        exit(0);
                }

                character = fgetc(file_ptr); //get next character from input file
                while (character == ' '){ //while at whitespace between meaningful indicators
                        character = fgetc(file_ptr); //get next character from input file
                }

                while (((character != EOF)&&(character != 'q')&&(character != '\n'))) { //continue looping until no more processes in queue
                        if(character != 'p'){ //check if retrieved character matchs expected character according to example input file format
                                printf("\n\nInput file is not in the correct format, process identifier 'p' expected.\n\n");
                                exit(0);
                        }
                        int process_num = 0; //variable to hold the process number read in from input file
                        fscanf(file_ptr, "%d", &process_num); //retrieve process number and store in variable
                        if((process_num < 1)||(process_num > INT_MAX)){ //check if retrieved value is a number as expected
                                printf("\n\nInput file is not in the correct format, invalid process number: %d.\n\n", process_num);
                                exit(0);
                        }
                        int burst_time = 0; //variable to hold the burst time of the process read in from input file
                        fscanf(file_ptr, "%d", &burst_time); //retrieve burst time and store in variable
                        if((burst_time < 1)||(burst_time > INT_MAX)){ //check if retrieved value is a number as expected
                                printf("\n\nInput file is not in the correct format, invalid burst time: %d.\n\n", burst_time);
                                exit(0);
                        }

                        Process process = {process_num, burst_time}; //create process object with process number and burst time
                        queues[queue_counter][process_counter] = process; //add process to proper queue in proper index

                        character = fgetc(file_ptr); //get next character from input file

                        while((character != EOF)&&(character != 'q')&&(character != 'p')||(character == ' ')){ //continue until next meaningful character
                                character = fgetc(file_ptr); //get next character from input file
                        }
                        process_counter++; //increment process counter to prepare for next process
                }
                process_counter = 0; //at the end of the queue, reset process counter for next queue
                queue_counter++; //increment queue counter to prepare for next queue
        }
        fclose(file_ptr); //close the file for good, we should now have stored all relevant info

		/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		* TESTING SCHEDULING ALGORITHMS WITH PROCESS FILLED QUEUES
		*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		*/

        file_ptr = fopen("cpu_scheduling_output_file.txt", "w"); // write only

        // test for files not existing.
        if (file_ptr == NULL) {
              printf("\n\nError opening file.\n\n");
              exit(0);
        }

        //loop to invoke each of the sceduling algorithms on each queue
        for (queue_counter = 0; queue_counter < num_of_queues; queue_counter++){ //for each queue
                printf("\n\n\n\nReady Queue %d Applying FCFS Scheduling:\n\n", queues_info[queue_counter].queue_number);
                fprintf(file_ptr, "\n\n\n\nReady Queue %d Applying FCFS Scheduling:\n\n", queues_info[queue_counter].queue_number);
                fcfs(queues[queue_counter], queue_process_count[queue_counter], file_ptr); //call first come first served

                printf("\n\n\n\nReady Queue %d Applying SJF Scheduling:\n\n", queues_info[queue_counter].queue_number);
                fprintf(file_ptr, "\n\n\n\nReady Queue %d Applying SJF Scheduling:\n\n", queues_info[queue_counter].queue_number);
                sjf(queues[queue_counter], queue_process_count[queue_counter], file_ptr); //call shortest job first

                printf("\n\n\n\nReady Queue %d Applying RR Scheduling:\n\n", queues_info[queue_counter].queue_number);
                fprintf(file_ptr, "\n\n\n\nReady Queue %d Applying RR Scheduling:\n\n", queues_info[queue_counter].queue_number);
                rr(queues[queue_counter], queue_process_count[queue_counter], queues_info[queue_counter].time_quantum, file_ptr); //call round robin

                printf("\n_______________________________________________________________________________________________________________________________________\n\n");
                fprintf(file_ptr, "\n_______________________________________________________________________________________________________________________________________\n\n");
        }
        fclose(file_ptr);
}

void fcfs(Process array[], int size, FILE * file_ptr){
        printf("Order of selection by CPU:\n");
        fprintf(file_ptr, "Order of selection by CPU:\n");
        int i;
        for(i = 0; i < size; i++){
                printf("p%d ", array[i].process_number);
                fprintf(file_ptr, "p%d ", array[i].process_number);
        }
        printf("\n\nIndividual waiting times for each process:\n");
        fprintf(file_ptr, "\n\nIndividual waiting times for each process:\n");

        int wait_time = 0; //variable to hold individual wait time for each process
        int total = 0; //variable to keep track of total wait time to be used in average

        for(i = 0; i < size; i++){ //for each process in queue
                printf("p%d = %d\n", array[i].process_number, wait_time);
                fprintf(file_ptr, "p%d = %d\n", array[i].process_number, wait_time);
                total = total + wait_time; //update total by burst time of process
                wait_time = wait_time + array[i].burst_time; //update wait time by process burst time
        }
        float average = ((float)total)/((float)size); //calculate average

        printf("\nAverage waiting time = %6.1f\n", average); //print average with 6 digits, 1 after the decimal
        fprintf(file_ptr, "\nAverage waiting time = %6.1f\n", average);
}

void sjf(Process array[], int size, FILE * file_ptr){
        Process * temp_array = malloc(sizeof(Process) * size); //temporary array to modify and rearrange
        int i;
        for(i = 0; i < size; i++){ //loop to populate new array
                temp_array[i] = array[i];
        }
         //Generic Selection Sort algorithm used to order processes based on burst time
        int j, position;
        Process swap; //process struct to be used to rearrange
        for (i = 0; i < (size - 1); i++){
                position = i;
                for (j = i + 1; j < size; j++){
                        if (temp_array[position].burst_time > temp_array[j].burst_time)
                                position = j;
                }
                if (position != i){
                        swap = temp_array[i];
                        temp_array[i] = temp_array[position];
                        temp_array[position] = swap;
                }
        }
        //by this point temp_array is ordered smallest to largest burst time
        printf("Order of selection by CPU:\n");
        fprintf(file_ptr, "Order of selection by CPU:\n");

        for(i = 0; i < size; i++){
                printf("p%d ", temp_array[i].process_number);
                fprintf(file_ptr, "p%d ", temp_array[i].process_number);
        }

        printf("\n\nIndividual waiting times for each process:\n");
        fprintf(file_ptr, "\n\nIndividual waiting times for each process:\n");

        //exact same methodology as fcfs from here
        int wait_time = 0;
        int total = 0;
        for(i = 0; i < size; i++){
                printf("p%d = %d\n", temp_array[i].process_number, wait_time);
                fprintf(file_ptr, "p%d = %d\n", temp_array[i].process_number, wait_time);
                total = total + wait_time;
                wait_time = wait_time + temp_array[i].burst_time;
        }
        float average = ((float)total)/((float)size);

        printf("\nAverage waiting time = %6.1f\n", average);
        fprintf(file_ptr, "\nAverage waiting time = %6.1f\n", average);
        free(temp_array);
}

void rr(Process array[], int size, int time_quantum, FILE * file_ptr){
        int total_required_cycles = 0; //variable to hold the total amount of required time quantum cycles (including requeueing)
        int required_requeue_count = 0; //variable to hold the number of processes that will require requeueing

        int * turnaround_times = malloc(sizeof(int) * size); //integer array that will be populated with the turnaround times of
                                                             //of each process, with turnaround_times[1] being the turnaround
                                                             //time of process 2 for example
        int i;
        for(i = 0; i < size; i++){ //loop to calculate the total amount of time quantum cycles that will be needed
                int process_required_cycles = 1; //variable to hold the number of time quantum cycles needed for a single process
                int burst_time = array[i].burst_time;
                if(burst_time <= time_quantum){ //check if burst time or time quantum is smaller
                        turnaround_times[i] = burst_time; //if burst time is smaller, turnaround time for this process is burst time
                }
                else{
                        turnaround_times[i] = 0; //if burst time is larger, we will set to 0 for now and calculate later
                        required_requeue_count++;
                }
                while(burst_time > time_quantum){ //if burst time is larger, see how many time quantums it will take
                        burst_time = burst_time - time_quantum; //
                        process_required_cycles++;
                }
                total_required_cycles = total_required_cycles + process_required_cycles; //update total required by process required
        }

        Process * temp_array = malloc(sizeof(Process) * total_required_cycles); //array of process structs that reflects total order of
                                                                                //processes, including requeues

        Process * overflow_processes = malloc(sizeof(Process) * required_requeue_count); //array of process structs that reflects only
                                                                                         //the processes that require requeue
        //loop to populate requeue process array
        int index = 0; //variable to keep track of place to insert processes in the overflow_array
        for(i = 0; i < size; i++){ //for each process in the original queue, check if it requires requeue
                int burst_time = array[i].burst_time;
                if(burst_time > time_quantum){ //if requeue is required (ie. burst time is more than one time quantum)
                        overflow_processes[index] = array[i]; //insert into overflow array
                        overflow_processes[index].burst_time = burst_time - time_quantum; //update the burst time of requeued process
                                                                                          //since when requeued one time quantums worth
                                                                                          //of work has been completed
                        index++; //increment index for next requeued process
                }
        }

        //loop to populate initial part of overall process array
        for(i = 0; i < size; i++){ //for each process in original array, they run first at least once so add to overall once
                temp_array[i] = array[i];
                int burst_time = temp_array[i].burst_time;
                if(burst_time <= time_quantum){
                        temp_array[i].burst_time = burst_time - time_quantum;
                }
        }

        //loop to requeue processes that require requeuing into the overall array in order and for the amount of requeues required
        int loop = 1; //variable to control conditional statement of overall loop
        int insert_position = size; //begin requeueing into overall array after all of the original processes
        while(loop){
                for(i = 0; i < index; i++){ //for each process in overflow array
                        int process_number = overflow_processes[i].process_number;
                        int remaining_time = overflow_processes[i].burst_time;
                        if(remaining_time > 0){ //if they still require processor time to be completed
                                temp_array[insert_position] = overflow_processes[i]; //requeue process
                                overflow_processes[i].burst_time = remaining_time - time_quantum; //update burst time leftover in new entry
                                insert_position++; //increment position to insert next requeue
                        }
                }
                loop = 0; //assume that all processes are complete and loop is done
                for(i = 0; i < index; i++){ //check all processes in overflow array
                        int remaining_time = overflow_processes[i].burst_time;
                        if(remaining_time > 0){ //if any process still requires more processor time, loop again
                                loop = 1;
                        }
                }
        }

        printf("Order of selection by CPU:\n");
        fprintf(file_ptr, "Order of selection by CPU:\n");

        //loop to print the total order
        for (i = 0; i < total_required_cycles; i++){
                printf("P%d ", temp_array[i].process_number);
                fprintf(file_ptr, "P%d ", temp_array[i].process_number);
        }

        printf("\n");
        fprintf(file_ptr, "\n");

        //loop to calculate the turnaround times for each process
        int proc_num;
        for (proc_num = 0; proc_num < size; proc_num++){ //for each process in original queue array
                if(turnaround_times[proc_num] == 0){ //if the turnaround time was not simply 1 burst time
                        int first_index = 0; //variable to hold first occurance of process in overall array
                        int last_index = 0; //variable to hold last occurance of process in overall array
                        int j;
                        for(j = 0; j < total_required_cycles; j++){ //iterate through overall array from beginning to find first
                                if(temp_array[j].process_number == proc_num+1){
                                        first_index = j;
                                        break;
                                }
                        }
                        for(j = total_required_cycles-1; j > 0; j--){ //iterate through overall array from end to find last
                                if(temp_array[j].process_number == proc_num+1){
                                        last_index = j;
                                        break;
                                }
                        }
                        //loop to calculate turnaround from first to last occurance
                        int turnaround = 0;
                        for(j = first_index; j <= last_index; j++){ //from first to last
                                int process_time_remaining = temp_array[j].burst_time;
                                if(process_time_remaining > 0){ //if process between first and last was not complete already
                                        if(process_time_remaining < time_quantum){ //if process between first and last used less than a full time quantum
                                                turnaround = turnaround + process_time_remaining; //update turnaround by process used time (burst time)
                                        }
                                        else{ //if process between first and last used a full time quantum
                                                turnaround = turnaround + time_quantum;  //update turnaround by process used time (time quantum)
                                        }
                                }
                                else{ //if process between first and last was complete already
                                        turnaround = turnaround + time_quantum + process_time_remaining; //update turnaround by process used time
                                }
                        }
                        turnaround_times[proc_num] = turnaround; //finalize turnaround time for process
                }
        }

        //loop to print turnaround times in numerical order
        printf("\nTurnaround times for each process:\n");
        fprintf(file_ptr, "\nTurnaround times for each process:\n");
        for (i = 0; i < size; i++){
                printf("P%d = %d\n", array[i].process_number, turnaround_times[i]);
                fprintf(file_ptr, "P%d = %d\n", array[i].process_number, turnaround_times[i]);
        }
        free(turnaround_times);
        free(temp_array);
        free(overflow_processes);
}