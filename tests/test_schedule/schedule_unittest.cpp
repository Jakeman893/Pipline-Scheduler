// Copyright 2006, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>

#include "gtest/gtest.h"
#include "../../src/pipeline.h"

int32_t  PIPE_WIDTH=1;
int32_t   NUM_REST_ENTRIES=8;
int32_t   NUM_ROB_ENTRIES=8;
int32_t   LOAD_EXE_CYCLES=4;
int32_t   SCHED_POLICY=0;

Inst_Info mockInst;

Pipeline *t;

// The order of the REST instructions
int order[] = {0, 1, 3, 4, 9, 2, 6, 8, 5, 7};

// Test initialization of pipeline function
TEST(InOrderTest, InitFunct) {
    t = pipe_init(NULL);
    EXPECT_FALSE(t == NULL);
}

// Check when REST empty
TEST(InOrderTest, EmptyRESTCheck) {
    pipe_cycle_schedule(t);
    EXPECT_FALSE(t->SC_latch[0].valid);
    // Fill REST for other tests
    int res;
    for(int i = 0; i < NUM_REST_ENTRIES; i++) {
        mockInst.inst_num = i;
        mockInst.dest_reg = i;
        mockInst.src1_reg = i % 2;
        mockInst.src1_tag = -1;
        mockInst.src2_reg = (i % 2) + 1;
        mockInst.src2_tag = -1;
        res = ROB_insert(t->pipe_ROB, mockInst);
        mockInst.dr_tag = res;
        RAT_set_remap(t->pipe_RAT, mockInst.dest_reg, res);
        REST_insert(t->pipe_REST, mockInst);
    }
}

// Instruction in REST is ready to be scheduled
TEST(InOrderTest, NoStalledInstruction) {
    REST_Entry* to_schedule = &t->pipe_REST->REST_Entries[0];
    to_schedule->inst.src1_ready = true;
    to_schedule->inst.src2_ready = true;
    pipe_cycle_schedule(t);
    Pipe_Latch* latch = &t->SC_latch[0];
    EXPECT_FALSE(latch->stall);
    EXPECT_TRUE(latch->valid);
    EXPECT_TRUE(to_schedule->scheduled);
    REST_remove(t->pipe_REST, to_schedule->inst);
}

// Instruction in REST is not ready to be scheduled
TEST(InOrderTest, StalledInstruction) {
    REST_Entry* to_schedule = &t->pipe_REST->REST_Entries[1];
    to_schedule->inst.src1_ready = false;
    to_schedule->inst.src2_ready = false;
    pipe_cycle_schedule(t);
    Pipe_Latch* latch = &t->SC_latch[0];
    EXPECT_TRUE(latch->stall);
    EXPECT_FALSE(latch->valid);
    EXPECT_FALSE(to_schedule->scheduled);
    REST_remove(t->pipe_REST, to_schedule->inst);
}

// Later instruction ready, not selected
TEST(InOrderTest, StalledNextReady) {
    REST_Entry* to_schedule = &t->pipe_REST->REST_Entries[3];
    to_schedule->inst.src1_ready = true;
    to_schedule->inst.src2_ready = true;

    to_schedule = &t->pipe_REST->REST_Entries[2];
    to_schedule->inst.src1_ready = false;
    to_schedule->inst.src2_ready = false;
    pipe_cycle_schedule(t);
    Pipe_Latch* latch = &t->SC_latch[0];
    EXPECT_TRUE(latch->stall);
    EXPECT_FALSE(latch->valid);
    EXPECT_FALSE(to_schedule->scheduled);
    REST_remove(t->pipe_REST, t->pipe_REST->REST_Entries[3].inst);
    REST_remove(t->pipe_REST, to_schedule->inst);
}

// REST entry not in order REST[0] inst_num=2 and REST[1] inst_num=1
TEST(InOrderTest, NotStalledRESTOrder) {
    REST_Entry* to_schedule = &t->pipe_REST->REST_Entries[0];
    to_schedule->inst.inst_num = 2;
    to_schedule->valid = true;
    to_schedule->scheduled = false;
    to_schedule->inst.src1_ready = true;
    to_schedule->inst.src2_ready = true;

    to_schedule = &t->pipe_REST->REST_Entries[1];
    to_schedule->inst.inst_num = 1;
    to_schedule->valid = true;
    to_schedule->scheduled = false;
    to_schedule->inst.src1_ready = true;
    to_schedule->inst.src2_ready = true;
    pipe_cycle_schedule(t);
    Pipe_Latch* latch = &t->SC_latch[0];
    EXPECT_FALSE(latch->stall);
    EXPECT_TRUE(latch->valid);
    EXPECT_TRUE(to_schedule->scheduled);
    REST_remove(t->pipe_REST, t->pipe_REST->REST_Entries[0].inst);
    REST_remove(t->pipe_REST, to_schedule->inst);
}

// REST Out of Order Schedule Policy setup
TEST(OutOfOrderTest, InitFunct) {
    delete t;
    SCHED_POLICY = 1;
    t = pipe_init(NULL);
    EXPECT_FALSE(t == NULL);
}

// Check when REST empty
TEST(OutOfOrderTest, EmptyRESTCheck) {
    pipe_cycle_schedule(t);
    EXPECT_FALSE(t->SC_latch[0].valid);
    // Fill REST for other tests
    int res;
    for(int i = 0; i < NUM_REST_ENTRIES; i++) {
        mockInst.inst_num = order[i];
        mockInst.dest_reg = i;
        mockInst.src1_reg = i % 2;
        mockInst.src1_tag = -1;
        mockInst.src2_reg = (i % 2) + 1;
        mockInst.src2_tag = -1;
        res = ROB_insert(t->pipe_ROB, mockInst);
        mockInst.dr_tag = res;
        RAT_set_remap(t->pipe_RAT, mockInst.dest_reg, res);
        REST_insert(t->pipe_REST, mockInst);
    }
}

// Check that if REST is in order, the correct instruction (oldest) is scheduled
TEST(OutOfOrderTest, InOrderReady) {
    REST_Entry* to_schedule = &t->pipe_REST->REST_Entries[1];
    to_schedule->valid = true;
    to_schedule->scheduled = false;
    to_schedule->inst.src1_ready = true;
    to_schedule->inst.src2_ready = true;

    to_schedule = &t->pipe_REST->REST_Entries[0];
    to_schedule->valid = true;
    to_schedule->scheduled = false;
    to_schedule->inst.src1_ready = true;
    to_schedule->inst.src2_ready = true;

    pipe_cycle_schedule(t);
    Pipe_Latch* latch = &t->SC_latch[0];
    EXPECT_FALSE(latch->stall);
    EXPECT_TRUE(latch->valid);
    EXPECT_TRUE(to_schedule->scheduled);
    EXPECT_EQ(latch->inst.inst_num, to_schedule->inst.inst_num);

    REST_remove(t->pipe_REST, to_schedule->inst);
    REST_remove(t->pipe_REST, t->pipe_REST->REST_Entries[1].inst);
}

// Check that if REST is out of order, the correct instruction (oldest) is scheduled
TEST(OutOfOrderTest, OutOfOrderReady) {
    REST_Entry* to_schedule = &t->pipe_REST->REST_Entries[3];
    to_schedule->valid = true;
    to_schedule->scheduled = false;
    to_schedule->inst.src1_ready = true;
    to_schedule->inst.src2_ready = true;

    to_schedule = &t->pipe_REST->REST_Entries[2];
    to_schedule->valid = true;
    to_schedule->scheduled = false;
    to_schedule->inst.src1_ready = true;
    to_schedule->inst.src2_ready = true;

    pipe_cycle_schedule(t);
    Pipe_Latch* latch = &t->SC_latch[0];
    EXPECT_FALSE(latch->stall);
    EXPECT_TRUE(latch->valid);
    EXPECT_TRUE(to_schedule->scheduled);
    EXPECT_EQ(latch->inst.inst_num, to_schedule->inst.inst_num);

    REST_remove(t->pipe_REST, to_schedule->inst);
    REST_remove(t->pipe_REST, t->pipe_REST->REST_Entries[3].inst);
}

// Check that REST without src being ready doesn't schedule
TEST(OutOfOrderTest, OutOfOrderNotReady) {
    REST_Entry* to_schedule = &t->pipe_REST->REST_Entries[5];
    to_schedule->valid = true;
    to_schedule->scheduled = false;
    to_schedule->inst.src1_ready = false;
    to_schedule->inst.src2_ready = true;

    to_schedule = &t->pipe_REST->REST_Entries[4];
    to_schedule->valid = true;
    to_schedule->scheduled = false;
    to_schedule->inst.src1_ready = true;
    to_schedule->inst.src2_ready = false;

    pipe_cycle_schedule(t);
    Pipe_Latch* latch = &t->SC_latch[0];
    EXPECT_TRUE(latch->stall);
    EXPECT_FALSE(latch->valid);

    REST_remove(t->pipe_REST, to_schedule->inst);
    REST_remove(t->pipe_REST, t->pipe_REST->REST_Entries[5].inst);
}

GTEST_API_ int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    delete t;
}
