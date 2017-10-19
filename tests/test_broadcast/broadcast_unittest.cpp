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
int32_t   NUM_REST_ENTRIES=16;
int32_t   NUM_ROB_ENTRIES=16;
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
TEST(BroadcastTest, EmptyStructsCheck) {
    pipe_cycle_broadcast(t);
    EXPECT_FALSE(t->EX_latch[0].valid);
    // Fill structs for other tests
    int res;
    for(int i = 0; i < NUM_REST_ENTRIES; i++) {
        mockInst.inst_num = i;
        mockInst.dest_reg = i % 8;
        mockInst.src1_reg = i % 4;
        mockInst.src1_tag = -1;
        mockInst.src2_reg = (i % 2) + 1;
        mockInst.src2_tag = -1;
        res = ROB_insert(t->pipe_ROB, mockInst);
        mockInst.dr_tag = res;
        RAT_set_remap(t->pipe_RAT, mockInst.dest_reg, res);
        REST_insert(t->pipe_REST, mockInst);
    }
}

// Check that valid instruction updates REST and ROB correctly
TEST(BroadcastTest, ValidInstUpdate) {
    mockInst.inst_num = 0;
    mockInst.dest_reg = 0;
    mockInst.src1_reg = 0;
    mockInst.src2_reg = 1;
    mockInst.dr_tag = 0;
    Pipe_Latch* ex = &t->EX_latch[0];
    ex->valid = true;
    ex->stall = false;
    ex->inst = mockInst;
    pipe_cycle_broadcast(t);
    EXPECT_TRUE(REST_check_space(t->pipe_REST));
    EXPECT_TRUE(ROB_check_ready(t->pipe_ROB, mockInst.dr_tag));
}

GTEST_API_ int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    delete t;
}
