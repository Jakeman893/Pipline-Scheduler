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
#include "../../src/rest.h"
#include "../../src/trace.h"

int32_t   NUM_REST_ENTRIES=32;

// Test initialization function
TEST(RestTest, InitFunct) {
    REST *t = REST_init();
    EXPECT_FALSE(t == NULL);
    delete t;
}

// Test that the REST check space returns true when empty
TEST(RestTest, CheckSpaceEmpty) {
    REST *t = REST_init();
    EXPECT_TRUE(REST_check_space(t));
    delete t;
}

// Test that inserting works
TEST(RestTest, InsertInstruction) {
    REST *t = REST_init();
    Inst_Info mockInst;
    REST_insert(t, mockInst);
    EXPECT_EQ(t->REST_Entries[0].inst.inst_num, mockInst.inst_num);
    delete t;
}

// Test that REST check space returns true when space is available
TEST(RestTest, CheckSpacePartFull) {
    REST *t = REST_init();
    Inst_Info mockInst;
    for(int i = 0; i < NUM_REST_ENTRIES - 1; i++)
        REST_insert(t, mockInst);
    EXPECT_TRUE(REST_check_space(t));
    delete t;
}

// test that a full REST returns false for check_space
TEST(RestTest, CheckSpaceFull) {
    REST *t = REST_init();
    Inst_Info mockInst;
    for(int i = 0; i < NUM_REST_ENTRIES; i++)
    {
        REST_insert(t, mockInst);
        ++mockInst.inst_num;
    }
    EXPECT_FALSE(REST_check_space(t));
    delete t;
}

// test that remove function accurately removes
TEST(RestTest, RemoveEntry) {
    REST *t = REST_init();
    Inst_Info mockInst;
    int i;
    for(i = 0; i < NUM_REST_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        REST_insert(t, mockInst);
    }
    EXPECT_FALSE(REST_check_space(t));
    mockInst.inst_num = 25;
    REST_remove(t, mockInst);
    EXPECT_FALSE(t->REST_Entries[25].valid);
    delete t;
}

// test that remove function accurately removes all entries
TEST(RestTest, RemoveAllEntry) {
    REST *t = REST_init();
    Inst_Info mockInst;
    int i;
    for(i = 0; i < NUM_REST_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        REST_insert(t, mockInst);
    }
    EXPECT_FALSE(REST_check_space(t));
    // REST_print_state(t);
    for(i = 0; i < NUM_REST_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        REST_remove(t, mockInst);
    }
    // REST_print_state(t);
    EXPECT_TRUE(REST_check_space(t));
    delete t;
}

// Test that wakeup instruction works without waking instruction
TEST(RestTest, SetTagNoWakeup) {
    REST *t = REST_init();
    Inst_Info mockInst;
    int i;
    for(i = 0; i < NUM_REST_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        mockInst.src1_reg = (i + 4) % 8;
        mockInst.src1_tag = (i % 16) * 32;
        mockInst.src2_reg = i % 4;
        mockInst.src2_tag = (i % 16) * 16;
        REST_insert(t, mockInst);
    }
    EXPECT_FALSE(REST_check_space(t));
    // REST_print_state(t);
    REST_wakeup(t, 96);
    EXPECT_TRUE(t->REST_Entries[3].inst.src1_ready && t->REST_Entries[3].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[6].inst.src2_ready && t->REST_Entries[6].inst.src2_tag == -1);    
    EXPECT_TRUE(t->REST_Entries[19].inst.src1_ready && t->REST_Entries[19].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[22].inst.src2_ready && t->REST_Entries[22].inst.src2_tag == -1);    
    // REST_print_state(t);
    delete t;
}

// Test that wakeup instruction works with waking instruction
TEST(RestTest, SetTagWakeup) {
    REST *t = REST_init();
    Inst_Info mockInst;
    int i;
    for(i = 0; i < NUM_REST_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        mockInst.src1_reg = (i + 4) % 8;
        mockInst.src1_tag = (i % 16) * 32;
        mockInst.src2_reg = i % 4;
        mockInst.src2_tag = (i % 16) * 16;
        REST_insert(t, mockInst);
    }
    EXPECT_FALSE(REST_check_space(t));
    // REST_print_state(t);
    REST_wakeup(t, 96);
    EXPECT_TRUE(t->REST_Entries[3].inst.src1_ready && t->REST_Entries[3].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[6].inst.src2_ready && t->REST_Entries[6].inst.src2_tag == -1);    
    EXPECT_TRUE(t->REST_Entries[19].inst.src1_ready && t->REST_Entries[19].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[22].inst.src2_ready && t->REST_Entries[22].inst.src2_tag == -1);    
    // REST_print_state(t);
    REST_wakeup(t, 48);
    // REST_print_state(t);
    EXPECT_TRUE(t->REST_Entries[3].inst.src2_ready && t->REST_Entries[3].inst.src2_tag == -1);
    EXPECT_TRUE(t->REST_Entries[19].inst.src2_ready && t->REST_Entries[19].inst.src2_tag == -1);
    delete t;
}

// Test that scheduling occurs if both sources are ready
TEST(RestTest, SetSchedule) {
    REST *t = REST_init();
    Inst_Info mockInst;
    int i;
    for(i = 0; i < NUM_REST_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        mockInst.src1_reg = (i + 4) % 8;
        mockInst.src1_tag = (i % 16) * 32;
        mockInst.src2_reg = i % 4;
        mockInst.src2_tag = (i % 16) * 16;
        REST_insert(t, mockInst);
    }
    EXPECT_FALSE(REST_check_space(t));
    // REST_print_state(t);
    REST_wakeup(t, 96);
    EXPECT_TRUE(t->REST_Entries[3].inst.src1_ready && t->REST_Entries[3].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[6].inst.src2_ready && t->REST_Entries[6].inst.src2_tag == -1);    
    EXPECT_TRUE(t->REST_Entries[19].inst.src1_ready && t->REST_Entries[19].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[22].inst.src2_ready && t->REST_Entries[22].inst.src2_tag == -1);    
    // REST_print_state(t);
    REST_wakeup(t, 48);
    EXPECT_TRUE(t->REST_Entries[3].inst.src2_ready && t->REST_Entries[3].inst.src2_tag == -1);
    EXPECT_TRUE(t->REST_Entries[19].inst.src2_ready && t->REST_Entries[19].inst.src2_tag == -1);
    mockInst = t->REST_Entries[19].inst;
    REST_schedule(t, mockInst);
    EXPECT_TRUE(t->REST_Entries[19].scheduled);
    delete t;
}

// Test that scheduling doesnt occur if only source 1 is ready
TEST(RestTest, NoSetScheduleSrc1) {
    REST *t = REST_init();
    Inst_Info mockInst;
    int i;
    for(i = 0; i < NUM_REST_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        mockInst.src1_reg = (i + 4) % 8;
        mockInst.src1_tag = (i % 16) * 32;
        mockInst.src2_reg = i % 4;
        mockInst.src2_tag = (i % 16) * 16;
        REST_insert(t, mockInst);
    }
    EXPECT_FALSE(REST_check_space(t));
    // REST_print_state(t);
    REST_wakeup(t, 96);
    EXPECT_TRUE(t->REST_Entries[3].inst.src1_ready && t->REST_Entries[3].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[6].inst.src2_ready && t->REST_Entries[6].inst.src2_tag == -1);    
    EXPECT_TRUE(t->REST_Entries[19].inst.src1_ready && t->REST_Entries[19].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[22].inst.src2_ready && t->REST_Entries[22].inst.src2_tag == -1);    
    // REST_print_state(t);
    mockInst = t->REST_Entries[19].inst;
    REST_schedule(t, mockInst);
    // REST_print_state(t);
    EXPECT_FALSE(t->REST_Entries[19].scheduled);
    delete t;
}

// Test that scheduling doesn't occur if only source 2 is ready
TEST(RestTest, NoSetScheduleSrc2) {
    REST *t = REST_init();
    Inst_Info mockInst;
    int i;
    for(i = 0; i < NUM_REST_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        mockInst.src1_reg = (i + 4) % 8;
        mockInst.src1_tag = (i % 16) * 32;
        mockInst.src2_reg = i % 4;
        mockInst.src2_tag = (i % 16) * 16;
        REST_insert(t, mockInst);
    }
    EXPECT_FALSE(REST_check_space(t));
    // REST_print_state(t);
    REST_wakeup(t, 96);
    EXPECT_TRUE(t->REST_Entries[3].inst.src1_ready && t->REST_Entries[3].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[6].inst.src2_ready && t->REST_Entries[6].inst.src2_tag == -1);    
    EXPECT_TRUE(t->REST_Entries[19].inst.src1_ready && t->REST_Entries[19].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[22].inst.src2_ready && t->REST_Entries[22].inst.src2_tag == -1);    
    // REST_print_state(t);
    REST_wakeup(t, 48);
    EXPECT_TRUE(t->REST_Entries[3].inst.src2_ready && t->REST_Entries[3].inst.src2_tag == -1);
    EXPECT_TRUE(t->REST_Entries[19].inst.src2_ready && t->REST_Entries[19].inst.src2_tag == -1);
    mockInst = t->REST_Entries[22].inst;
    REST_schedule(t, mockInst);
    EXPECT_FALSE(t->REST_Entries[22].scheduled);
    delete t;
}

// Test that scheduling doesn't occur if neither source ready
TEST(RestTest, NoSetSchedule) {
    REST *t = REST_init();
    Inst_Info mockInst;
    int i;
    for(i = 0; i < NUM_REST_ENTRIES; i++)
    {
        mockInst.inst_num = i;
        mockInst.src1_reg = (i + 4) % 8;
        mockInst.src1_tag = (i % 16) * 32;
        mockInst.src2_reg = i % 4;
        mockInst.src2_tag = (i % 16) * 16;
        REST_insert(t, mockInst);
    }
    EXPECT_FALSE(REST_check_space(t));
    // REST_print_state(t);
    REST_wakeup(t, 96);
    EXPECT_TRUE(t->REST_Entries[3].inst.src1_ready && t->REST_Entries[3].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[6].inst.src2_ready && t->REST_Entries[6].inst.src2_tag == -1);    
    EXPECT_TRUE(t->REST_Entries[19].inst.src1_ready && t->REST_Entries[19].inst.src1_tag == -1);
    EXPECT_TRUE(t->REST_Entries[22].inst.src2_ready && t->REST_Entries[22].inst.src2_tag == -1);    
    // REST_print_state(t);
    REST_wakeup(t, 48);
    EXPECT_TRUE(t->REST_Entries[3].inst.src2_ready && t->REST_Entries[3].inst.src2_tag == -1);
    EXPECT_TRUE(t->REST_Entries[19].inst.src2_ready && t->REST_Entries[19].inst.src2_tag == -1);
    mockInst = t->REST_Entries[12].inst;
    REST_schedule(t, mockInst);
    EXPECT_FALSE(t->REST_Entries[12].scheduled);
    delete t;
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
