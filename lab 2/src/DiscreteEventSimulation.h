/* 



*/

#include "classes/Event.h"

class DiscreteEventSimulation{

    public:
    Event* get_event();
    void put_event();
    void rm_event();
    int get_next_event_time();

};