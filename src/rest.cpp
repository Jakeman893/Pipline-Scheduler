#include <stdio.h>
#include <assert.h>

#include "rest.h"

extern int32_t NUM_REST_ENTRIES;

/////////////////////////////////////////////////////////////
// Init function initializes the Reservation Station
/////////////////////////////////////////////////////////////

REST* REST_init(void){
  int ii;
  REST *t = (REST *) calloc (1, sizeof (REST));
  for(ii=0; ii<MAX_REST_ENTRIES; ii++){
    t->REST_Entries[ii].valid=false;
  }
  assert(NUM_REST_ENTRIES<=MAX_REST_ENTRIES);
  return t;
}

////////////////////////////////////////////////////////////
// Print State
/////////////////////////////////////////////////////////////
void REST_print_state(REST *t){
 int ii = 0;
  printf("Printing REST \n");
  printf("Entry  Inst Num  S1_tag S1_ready S2_tag S2_ready  Vld Scheduled\n");
  for(ii = 0; ii < NUM_REST_ENTRIES; ii++) {
    printf("%5d ::  \t\t%d\t", ii, (int)t->REST_Entries[ii].inst.inst_num);
    printf("%5d\t\t", t->REST_Entries[ii].inst.src1_tag);
    printf("%5d\t\t", t->REST_Entries[ii].inst.src1_ready);
    printf("%5d\t\t", t->REST_Entries[ii].inst.src2_tag);
    printf("%5d\t\t", t->REST_Entries[ii].inst.src2_ready);
    printf("%5d\t\t", t->REST_Entries[ii].valid);
    printf("%5d\n", t->REST_Entries[ii].scheduled);
    }
  printf("\n");
}

/////////////////////////////////////////////////////////////
//------- DO NOT CHANGE THE CODE ABOVE THIS LINE -----------
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// If space return true else return false
/////////////////////////////////////////////////////////////

bool REST_check_space(REST *t){
    int i = 0;
    // Iterate through all REST entries
    for(i; i < NUM_REST_ENTRIES; i++)
        // If an entry is not valid, immediately return true
        if(!t->REST_Entries[i].valid)
            return true;
    return false;
}

/////////////////////////////////////////////////////////////
// Insert an inst in REST, must do check_space first
/////////////////////////////////////////////////////////////

void  REST_insert(REST *t, Inst_Info inst){
    // If there is no space, don't insert
    if(!REST_check_space(t))
        return;
    
    // Iterate through REST (again) to find a non-valid entry
    int i = 0;
    REST_Entry* entry = NULL;
    for(i; i < NUM_REST_ENTRIES; i++)
    {
        // When invalid entry found, set to inst 
        entry = &t->REST_Entries[i];
        if(!entry->valid)
        {
            entry->valid = true;
            entry->scheduled = false;
            entry->inst = inst;
            return;
        }
    }
}

/////////////////////////////////////////////////////////////
// When instruction finishes execution, remove from REST
/////////////////////////////////////////////////////////////

void  REST_remove(REST *t, Inst_Info inst){
    // If REST is empty, obviously nothing to do
    if(!REST_check_space(t))
        return;
    
    // Search REST for inst instance matching passed in inst
    int i = 0;
    REST_Entry* entry = NULL;
    for(i; i < NUM_REST_ENTRIES; i++)
    {
        // When item found (comparing by inst_num as opposed to var by var)
        //  mark as invalid
        entry = &t->REST_Entries[i];
        if(entry->inst.inst_num == inst.inst_num)
        {
            entry->valid = false;
            return;
        }
    }
}

/////////////////////////////////////////////////////////////
// For broadcast of freshly ready tags, wakeup waiting inst
/////////////////////////////////////////////////////////////

void  REST_wakeup(REST *t, int tag){
    // If REST empty, obviously nothing to do
    if(!REST_check_space(t))
        return;

    // Search REST for inst instance with matching tags for src1 or src2
    int i = 0;
    REST_Entry* entry = NULL;
    for(i; i < NUM_REST_ENTRIES; i++)
    {
        entry = &t->REST_Entries[i];
        // When entry with tag found, mark R1 or R2 based on match
        if(entry->inst.src1_tag == tag)
        {
            entry->inst.src1_tag = READY_SRC_TAG;
            entry->inst.src1_ready = true;
        }
        if(entry->inst.src2_tag == tag)
        {
            entry->inst.src2_tag = READY_SRC_TAG;
            entry->inst.src2_ready = true;
        }
    }
}

/////////////////////////////////////////////////////////////
// When an instruction gets scheduled, mark REST entry as such
/////////////////////////////////////////////////////////////

void  REST_schedule(REST *t, Inst_Info inst){
    // If REST empty, obviously nothing to do
    if(!REST_check_space(t))
        return;
    
    // Search REST for entry with matching inst_num
    int i = 0;
    REST_Entry* entry = NULL;
    for(i; i < NUM_REST_ENTRIES; i++)
    {
        entry = &t->REST_Entries[i];
        // Set entry as scheduled only if valid and both src1 and src2 are ready
        if(entry->inst.inst_num == inst.inst_num && entry->valid)
        {
            entry->scheduled = entry->inst.src1_ready && entry->inst.src2_ready;
            return;
        }
    }
}
