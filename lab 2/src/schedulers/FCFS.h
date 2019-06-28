#include "scheduler_interface.h"

class FCFS : public SchedulerInterface{
    public:
    void add_process();
    void get_next_process();
};