#ifndef _RAT_H_
#define _RAT_H_
#include <inttypes.h>
#include <cstdlib>
#define MAX_ARF_REGS 32

// Entry for Register Alias Table 
typedef struct RAT_Entry_Struct {
    // Valid bit 
    // (register not renamed and value can be read from Architecture Register File right away) 
    bool valid;
    // Physical register file id
    // Physical Register file represents registers actually on chip
    uint64_t prf_id;
}RAT_Entry;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

typedef struct RAT {
    // Map from Architecture Register File ID to Physical Register File ID 
    RAT_Entry  RAT_Entries[MAX_ARF_REGS];
}RAT;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

RAT* RAT_init(void);
void RAT_print_state(RAT *t);

// Given Architecture Register File ID, get Physical Register File ID 
int  RAT_get_remap(RAT *t, int arf_id);
void RAT_set_remap(RAT *t, int arf_id, int prf_id);
void RAT_reset_entry(RAT *t, int arf_id);


#endif

