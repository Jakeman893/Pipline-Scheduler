#ifndef _REST_H_
#define _REST_H_
#include <inttypes.h>
#include <assert.h>
#include <cstdlib>
#include "trace.h"

#define MAX_REST_ENTRIES 256
#define READY_SRC_TAG -1

// Reservation Station

// Reservation Station Entry
typedef struct REST_Entry_Struct {
    // Valid bit for entry
    bool valid;
    // Scheduled bit for entry
    bool scheduled;
    // Instruction Information Record
    Inst_Info inst;
} REST_Entry;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

typedef struct REST {
    // The actual REST table 
    REST_Entry  REST_Entries[MAX_REST_ENTRIES];
} REST;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

REST* REST_init(void);
void  REST_print_state(REST *t);

bool  REST_check_space(REST *t);
void  REST_insert(REST *t, Inst_Info inst);
void  REST_remove(REST *t, Inst_Info inst);
void  REST_wakeup(REST *t, int tag);
void  REST_schedule(REST *t, Inst_Info inst);

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


#endif

