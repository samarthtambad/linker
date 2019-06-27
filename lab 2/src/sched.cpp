#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>

using namespace std;

bool printVerbose = false;
bool printTrace = false;
bool printEventQueue = false;

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

// struct Event{
//     Process *evtProcess;
//     int evtTimeStamp;
// };

// struct Process{

// };

// Event* get_event(){
//     return nullptr;
// }

// void Simulation(){
//     Event *evt;
//     while(evt = get_event()){
//         Process *proc = evt->evtProcess;
//         int CURRENT_TIME = evt->evtTimeStamp;
//     }
// }


int main(int argc, char *argv[]){
    
    int c;
    string schedspec;
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
                return 1;
            default:
                abort();
        }
    }

    printf("printVerbose: %d,printTrace: %d,printEventQueue: %d, SchedSpec: %s\n", printVerbose, printTrace, printEventQueue, schedspec.c_str());
    
    for (int index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);

    return 0;
}