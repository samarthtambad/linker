#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <queue>

#include "DiscreteEventSimulation.h"
#include "classes/Event.h"
#include "classes/Process.h"

using namespace std;

// states of process
typedef enum {
    STATE_CREATED, // 0
    STATE_READY,    // 1
    STATE_RUNNING,  // 2
    STATE_BLOCKED   // 3
} process_state_t;

// states to transition to
typedef enum {
    TRANS_TO_READY,
    TRANS_TO_RUN,
    TRANS_TO_BLOCK,
    TRANS_TO_PREEMPT
} state_transition_t;

bool printVerbose = false;
bool printTrace = false;
bool printEventQueue = false;
int sim_time = 0;


// figure out the purpose of these
int CURRENT_TIME = 0;
bool CALL_SCHEDULER = true;
Process* CURRENT_RUNNING_PROCESS;
int timeInPrevState;

queue<Event> event_queue;
static DiscreteEventSimulation simulation;

void Simulation(){
    Event *evt;
    while(evt = simulation.get_event()){
        Process *proc = evt->evtProcess;
        CURRENT_TIME = evt->evtTimeStamp;
        timeInPrevState = CURRENT_TIME - proc -> state_ts;

        switch (evt->transition){
        case TRANS_TO_READY:
            
            break;
        case TRANS_TO_RUN:
            
            break;
        case TRANS_TO_BLOCK:
            
            break;
        case TRANS_TO_PREEMPT:
            
            break;
        }
        
        // remove current event object from Memory
        delete evt;
        evt = nullptr;

        if(CALL_SCHEDULER){
            if(simulation.get_next_event_time() == CURRENT_TIME){
                continue;   // process next event from event queue
            }
            CALL_SCHEDULER = false; // reset global flag
            if(CURRENT_RUNNING_PROCESS == nullptr){
                CURRENT_RUNNING_PROCESS = THE_SCHEDULER->get_next_process();
                if(CURRENT_RUNNING_PROCESS == nullptr) continue;    // create event to make this process runnable for same time.
            }
        }

    }
}

void parse_args(int argc, char *argv[], string &input_file, string &rand_file, string &schedspec){
    int c;
    while ((c = getopt (argc, argv, "vtes:")) != -1){
        switch(c){
            case 'v': 
                printVerbose = true;
                break;
            case 't':
                printTrace = true;
                break;
            case 'e':
                printEventQueue = true;
                break;
            case 's':
                schedspec = optarg;
            break;
            case '?':
                if (optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            default:
                abort();
        }
    }

    int idx = optind;
    if(idx < argc) input_file = argv[idx];
    idx++;
    if(idx < argc) rand_file = argv[idx];
}

int main(int argc, char *argv[]){

    // parse arguments
    string input_file, rand_file, schedspec;
    parse_args(argc, argv, input_file, rand_file, schedspec);
    printf("printVerbose: %d,printTrace: %d,printEventQueue: %d, SchedSpec: %s\n", printVerbose, printTrace, printEventQueue, schedspec.c_str());
    printf ("Input: %s, Rand: %s \n", input_file.c_str(), rand_file.c_str());

    // parse input file
    FILE *fptr;
    fptr = fopen(input_file.c_str(), "r");
    if(fptr == NULL){
        printf("cannot open file");
    }



    fclose(fptr);

    return 0;
}