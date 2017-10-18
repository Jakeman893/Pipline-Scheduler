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
int32_t   NUM_REST_ENTRIES=32;
int32_t   NUM_ROB_ENTRIES=32;
int32_t   LOAD_EXE_CYCLES=4;
int32_t   SCHED_POLICY=1;

Inst_Info mockInst;

Pipeline *t = pipe_init(NULL);

// Test initialization of pipeline function
TEST(RenameTest, InitFunct) {
    t = pipe_init(NULL);
    EXPECT_FALSE(t == NULL);
}

// With empty RAT, ROB, and REST
TEST(RenameTest, NoRenameNoPrev) {
    mockInst.inst_num = 0;
    // An instruction in the form ADD R1, R2, R3
    mockInst.dest_reg = 1;
    mockInst.src1_reg = 2;
    mockInst.src1_tag = -1;
    mockInst.src2_reg = 3;
    mockInst.src2_tag = -1;
    Pipe_Latch *latch = &t->ID_latch[0];
    latch->valid = true;
    latch->stall = false;
    latch->inst = mockInst;
    pipe_cycle_rename(t);
    EXPECT_FALSE(latch->stall);
    EXPECT_EQ(0, latch->inst.dr_tag);
    EXPECT_TRUE(latch->inst.src1_ready);
    EXPECT_TRUE(latch->inst.src2_ready);
}

// With collision make sure dest_reg remaps to most recent phys reg
TEST(RenameTest, NoRenamePrevDest) {
    mockInst.inst_num = 1;
    // An instruction in the form ADD R1, R2, R3
    mockInst.dest_reg = 1;
    mockInst.src1_reg = 5;
    mockInst.src1_tag = -1;
    mockInst.src2_reg = 6;
    mockInst.src2_tag = -1;
    Pipe_Latch *latch = &t->ID_latch[0];
    latch->valid = true;
    latch->stall = false;
    latch->inst = mockInst;
    pipe_cycle_rename(t);
    EXPECT_FALSE(latch->stall);
    EXPECT_TRUE(latch->inst.src1_ready = true);
    EXPECT_TRUE(latch->inst.src2_ready = true);
    EXPECT_EQ(1, latch->inst.dr_tag);
    EXPECT_EQ(-1, latch->inst.src1_tag);
    EXPECT_EQ(-1, latch->inst.src2_tag);
}

// With collision make sure dest_reg remaps to diff phys reg
TEST(RenameTest, Src1RenamePrevDest) {
    mockInst.inst_num = 2;
    // An instruction in the form ADD R1, R2, R3
    mockInst.dest_reg = 5;
    mockInst.src1_reg = 1;
    mockInst.src1_tag = -1;
    mockInst.src1_ready = false;
    mockInst.src2_reg = 4;
    mockInst.src2_tag = -1;
    Pipe_Latch *latch = &t->ID_latch[0];
    latch->valid = true;
    latch->stall = false;
    latch->inst = mockInst;
    pipe_cycle_rename(t);
    EXPECT_FALSE(latch->stall);
    EXPECT_EQ(2, latch->inst.dr_tag);
    EXPECT_EQ(1, latch->inst.src1_tag);
    EXPECT_FALSE(latch->inst.src1_ready);
    EXPECT_EQ(-1, latch->inst.src2_tag);
}

// Test correct retrieval of src 1 tag
TEST(RenameTest, Src2RenamePrevDest) {
    mockInst.inst_num = 3;
    // An instruction in the form ADD R1, R2, R3
    mockInst.dest_reg = 3;
    mockInst.src1_reg = 7;
    mockInst.src1_tag = -1;
    mockInst.src2_reg = 5;
    mockInst.src2_tag = -1;
    mockInst.src2_ready = false;
    Pipe_Latch *latch = &t->ID_latch[0];
    latch->valid = true;
    latch->stall = false;
    latch->inst = mockInst;
    pipe_cycle_rename(t);
    EXPECT_FALSE(latch->stall);
    EXPECT_EQ(3, latch->inst.dr_tag);
    EXPECT_EQ(-1, latch->inst.src1_tag);
    EXPECT_EQ(2, latch->inst.src2_tag);
    EXPECT_FALSE(latch->inst.src2_ready);
}

// Test correct retrieval of both src tags
TEST(RenameTest, BothSrcRenamePrevDest) {
    mockInst.inst_num = 3;
    // An instruction in the form ADD R1, R2, R3
    mockInst.dest_reg = 3;
    mockInst.src1_reg = 3;
    mockInst.src1_tag = -1;
    mockInst.src2_reg = 5;
    mockInst.src2_tag = -1;
    Pipe_Latch *latch = &t->ID_latch[0];
    latch->valid = true;
    latch->stall = false;
    latch->inst = mockInst;
    // Update number of ROB entries for next test
    NUM_ROB_ENTRIES = 5;
    pipe_cycle_rename(t);
    EXPECT_FALSE(latch->stall);
    EXPECT_EQ(4, latch->inst.dr_tag);
    EXPECT_EQ(3, latch->inst.src1_tag);
    EXPECT_EQ(2, latch->inst.src2_tag);
}

// ROB
TEST(RenameTest, BothSrcRenameFullROB) {
    mockInst.inst_num = 4;
    // An instruction in the form ADD R1, R2, R3
    mockInst.dest_reg = 3;
    mockInst.src1_reg = 3;
    mockInst.src1_tag = -1;
    mockInst.src2_reg = 5;
    mockInst.src2_tag = -1;
    Pipe_Latch *latch = &t->ID_latch[0];
    latch->valid = true;
    latch->stall = false;
    latch->inst = mockInst;
    pipe_cycle_rename(t);
    EXPECT_TRUE(latch->stall);
    EXPECT_EQ(0, latch->inst.dr_tag);
}

// ROB
TEST(RenameTest, BothSrcRenameFullREST) {
    mockInst.inst_num = 4;
    // An instruction in the form ADD R1, R2, R3
    mockInst.dest_reg = 3;
    mockInst.src1_reg = 3;
    mockInst.src1_tag = -1;
    mockInst.src2_reg = 5;
    mockInst.src2_tag = -1;
    Pipe_Latch *latch = &t->ID_latch[0];
    latch->valid = true;
    latch->stall = false;
    latch->inst = mockInst;
    NUM_REST_ENTRIES=5;
    pipe_cycle_rename(t);
    EXPECT_TRUE(latch->stall);
    EXPECT_EQ(0, latch->inst.dr_tag);
}

// TODO: Tests for larger pipe width

GTEST_API_ int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    delete t;
}
