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
#include "../../src/rob.h"
#include "../../src/trace.h"

int32_t   NUM_ROB_ENTRIES=32;

// Test ROB initialization
TEST(RobTest, InitRob) {
    ROB* rob = ROB_init();
    EXPECT_FALSE(rob == NULL);
    delete rob;
}

// Test ROB empty check
TEST(RobTest, EmptyRobCheck) {
    ROB* rob = ROB_init();
    EXPECT_TRUE(ROB_check_space(rob));
    delete rob;
}

// Test ROB single insertion
TEST(RobTest, SingleInsert) {
    ROB* rob = ROB_init();
    Inst_Info mockInst;
    mockInst.inst_num = 0;
    ROB_insert(rob, mockInst);
    EXPECT_TRUE(rob->ROB_Entries[rob->head_ptr].valid);
    EXPECT_EQ(rob->ROB_Entries[rob->head_ptr].inst.inst_num, 0);
    delete rob;
}

// Test full insert and no space available
TEST(RobTest, InsertAlltInst) {
    ROB* rob = ROB_init();
    Inst_Info mockInst;
    for(int i = 0; i < NUM_ROB_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        ROB_insert(rob, mockInst);
        int prev_entry = int(rob->tail_ptr) - 1;
        if(prev_entry < 0)
            prev_entry += NUM_ROB_ENTRIES;
        EXPECT_TRUE(rob->ROB_Entries[prev_entry].valid);
        EXPECT_EQ(rob->ROB_Entries[prev_entry].inst.inst_num, i);
        EXPECT_EQ(prev_entry, i);
        ++mockInst.inst_num;
    }
    EXPECT_FALSE(ROB_check_space(rob));
    delete rob;
}

// Fill the ROB and remove all
TEST(RobTest, AddAllAndRemove) {
    ROB* rob = ROB_init();
    Inst_Info mockInst;
    for(int i = 0; i < NUM_ROB_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        int prev_entry = ROB_insert(rob, mockInst);
        EXPECT_TRUE(rob->ROB_Entries[prev_entry].valid);
        EXPECT_EQ(rob->ROB_Entries[prev_entry].inst.inst_num, i);
        EXPECT_EQ(prev_entry, i);
        ++mockInst.inst_num;
    }
    EXPECT_FALSE(ROB_check_space(rob));
    for(int i = 0; i < NUM_ROB_ENTRIES; i++)
    {
        mockInst = ROB_remove_head(rob);
        EXPECT_EQ(mockInst.inst_num, i);
        EXPECT_FALSE(rob->ROB_Entries[rob->head_ptr - 1].valid);
        EXPECT_FALSE(rob->ROB_Entries[rob->head_ptr - 1].ready);
    }
    EXPECT_EQ(rob->head_ptr, rob->tail_ptr);
    EXPECT_TRUE(ROB_check_space(rob));
    delete rob;
}

// Test mark instruction as ready
TEST(RobTest, MarkReady) {
    ROB* rob = ROB_init();
    Inst_Info mockInst;
    for(int i = 0; i < NUM_ROB_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        ROB_insert(rob, mockInst);
        int prev_entry = int(rob->tail_ptr) - 1;
        if(prev_entry < 0)
            prev_entry += NUM_ROB_ENTRIES;
        EXPECT_TRUE(rob->ROB_Entries[prev_entry].valid);
        EXPECT_EQ(rob->ROB_Entries[prev_entry].inst.inst_num, i);
        EXPECT_EQ(prev_entry, i);
        ++mockInst.inst_num;
    }
    EXPECT_FALSE(ROB_check_space(rob));
    mockInst = Inst_Info();
    mockInst.inst_num = 5;
    ROB_mark_ready(rob, mockInst);
    // ROB_print_state(rob);
    EXPECT_TRUE(rob->ROB_Entries[5].valid);
    EXPECT_TRUE(rob->ROB_Entries[5].ready);
    delete rob;
}

// Fill the ROB and remove all
TEST(RobTest, AddAllAndOneMore) {
    ROB* rob = ROB_init();
    Inst_Info mockInst;
    for(int i = 0; i < NUM_ROB_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        int prev_entry = ROB_insert(rob, mockInst);
        EXPECT_TRUE(rob->ROB_Entries[prev_entry].valid);
        EXPECT_EQ(rob->ROB_Entries[prev_entry].inst.inst_num, i);
        EXPECT_EQ(prev_entry, i);
        ++mockInst.inst_num;
    }
    EXPECT_FALSE(ROB_check_space(rob));
    ++mockInst.inst_num;
    EXPECT_EQ(ROB_insert(rob, mockInst), -1);
    for(int i = 0; i < NUM_ROB_ENTRIES; i++)
    {
        mockInst = ROB_remove_head(rob);
        EXPECT_EQ(mockInst.inst_num, i);
        EXPECT_FALSE(rob->ROB_Entries[rob->head_ptr - 1].valid);
        EXPECT_FALSE(rob->ROB_Entries[rob->head_ptr - 1].ready);
    }
    EXPECT_EQ(rob->head_ptr, rob->tail_ptr);
    EXPECT_TRUE(ROB_check_space(rob));
    delete rob;
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
