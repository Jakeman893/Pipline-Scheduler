#include <stdio.h>
#include <assert.h>

#include "rob.h"


extern int32_t NUM_ROB_ENTRIES;

/////////////////////////////////////////////////////////////
// Init function initializes the ROB
/////////////////////////////////////////////////////////////

ROB* ROB_init(void){
  int ii;
  ROB *t = (ROB *) calloc (1, sizeof (ROB));
  for(ii=0; ii<MAX_ROB_ENTRIES; ii++){
    t->ROB_Entries[ii].valid=false;
    t->ROB_Entries[ii].ready=false;
  }
  t->head_ptr=0;
  t->tail_ptr=0;
  return t;
}

/////////////////////////////////////////////////////////////
// Print State
/////////////////////////////////////////////////////////////
void ROB_print_state(ROB *t){
 int ii = 0;
  printf("Printing ROB \n");
  printf("Entry  Inst   Valid   ready\n");
  for(ii = 0; ii < 7; ii++) {
    printf("%5d ::  %d\t", ii, (int)t->ROB_Entries[ii].inst.inst_num);
    printf(" %5d\t", t->ROB_Entries[ii].valid);
    printf(" %5d\n", t->ROB_Entries[ii].ready);
  }
  printf("\n");
}

/////////////////////////////////////////////////////////////
//------- DO NOT CHANGE THE CODE ABOVE THIS LINE -----------
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// If there is space in ROB return true, else false
/////////////////////////////////////////////////////////////

bool ROB_check_space(ROB *t){
    // return false if full, return true otherwise
    return !((t->head_ptr == t->tail_ptr) && t->ROB_Entries[t->head_ptr].valid);
}

/////////////////////////////////////////////////////////////
// insert entry at tail, increment tail (do check_space first)
/////////////////////////////////////////////////////////////

int ROB_insert(ROB *t, Inst_Info inst){
    // Check if space available
    if(!ROB_check_space(t))
        return 0;

    // Insert at tail
    ROB_Entry *entry = &t->ROB_Entries[t->tail_ptr];
    entry->inst = inst;
    entry->valid = true;
    entry->ready = false;

    // Increment tail, wrapping if necessary
    t->tail_ptr = ++t->tail_ptr % NUM_ROB_ENTRIES;
}

/////////////////////////////////////////////////////////////
// Once an instruction finishes execution, mark rob entry as done
/////////////////////////////////////////////////////////////

void ROB_mark_ready(ROB *t, Inst_Info inst){
    // Iterate through table starting at head_ptr
    int i = 0;
    int idx;
    ROB_Entry *entry = NULL;
    for(i; i != t->tail_ptr; i++)
    {
        idx = (t->head_ptr + i) % NUM_ROB_ENTRIES;
        entry = &t->ROB_Entries[idx];
        // match inst_num with an entry
        if(entry->inst.inst_num == inst.inst_num)
            entry->ready = true;
    }
}

/////////////////////////////////////////////////////////////
// Find whether the prf-rob entry is ready
/////////////////////////////////////////////////////////////

bool ROB_check_ready(ROB *t, int tag){
    // look for tag in ROB and return if th ROB entry is ready
    int i = 0;
    int idx;
    ROB_Entry *entry = NULL;
    for(i; i != t->tail_ptr; i++)
    {
        idx = (t->head_ptr + i) % NUM_ROB_ENTRIES;
        entry = &t->ROB_Entries[idx];
        // Match tag with an entry
        if(entry->inst.src1_tag == tag || entry->inst.src2_tag == tag)
            return entry->ready;
    }
}


/////////////////////////////////////////////////////////////
// Check if the oldest ROB entry is ready for commit
/////////////////////////////////////////////////////////////

bool ROB_check_head(ROB *t){
    const ROB_Entry first = t->ROB_Entries[t->head_ptr];
    return first.ready && first.valid;
}

/////////////////////////////////////////////////////////////
// Remove oldest entry from ROB (after ROB_check_head)
/////////////////////////////////////////////////////////////

Inst_Info ROB_remove_head(ROB *t){
    ROB_Entry* first = &t->ROB_Entries[t->head_ptr];
    Inst_Info ret = first->inst;
    t->head_ptr = (1 + t->head_ptr) % NUM_ROB_ENTRIES;
    return ret;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
