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
#include "../../src/rat.h"

// Test That making instance works correctly
TEST(RatTest, MakeInstance) {
    RAT *rat = RAT_init();
    EXPECT_FALSE(rat == NULL);
    delete rat;
}

// Test that when retrieving from empty RAT,
//  the retrieved value is equal to value given
TEST(RatTest, RetrieveEmptyRat) {
    RAT *rat = RAT_init();
    int reg = RAT_get_remap(rat, 1);
    EXPECT_EQ(reg, 1);
    delete rat;
}

// Test that when RAT has value set, the RAT returns
//  mapped register.
TEST(RatTest, SetRatEntry) {
    RAT *rat = RAT_init();
    RAT_set_remap(rat, 1, 5);
    int reg = RAT_get_remap(rat, 1);
    EXPECT_EQ(reg, 5);
    delete rat;
}

// Test that when RAT has value reset, the RAT returns
//  the passed value
TEST(RatTest, ResetRatEntry) {
    RAT *rat = RAT_init();
    RAT_set_remap(rat, 1, 5);
    int reg = RAT_get_remap(rat, 1);
    EXPECT_EQ(reg, 5);
    RAT_reset_entry(rat, 1);
    reg = RAT_get_remap(rat, 1);
    EXPECT_EQ(reg, 1);
}

// Test when all values set in RAT, all valid bits true
TEST(RatTest, ValidBitTest) {
    RAT *rat = RAT_init();
    int i = 0;
    for(;i < MAX_ARF_REGS;i++)
    {
        RAT_set_remap(rat, i, i * 5);
        EXPECT_TRUE(rat->RAT_Entries[i].valid);
    }
}

// Test that prf_id is set when remapping
TEST(RatTest, PRFTest) {
    RAT *rat = RAT_init();
    int i = 0;
    for(;i < MAX_ARF_REGS;i++)
    {
        RAT_set_remap(rat, i, i * 5);
        EXPECT_EQ(rat->RAT_Entries[i].prf_id, i * 5);
    }
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
