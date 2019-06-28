/* 


*/

#include "Process.h"

class Event{

    public:
    int evtTimeStamp;
    Process* evtProcess;
    int oldstate;
    int newstate;
    int transition;
};