/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
/*****************************************************************************/
class TestIntegerMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestIntegerMoveGenerator, setup) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 20);

    for (auto i = 0; i < 10; i++) {
        x(i) = 2 * i;
    }
    x(0).fix();
    x(9).fix();

    model.categorize_variables();
    model.categorize_constraints();

    auto integer_variable_ptrs =
        model.variable_reference().integer_variable_ptrs;

    model.neighborhood().integer().setup(integer_variable_ptrs);
    model.neighborhood().integer().update_moves(true, false, false, false);

    auto& moves = model.neighborhood().integer().moves();
    auto& flags = model.neighborhood().integer().flags();
    EXPECT_EQ(32, static_cast<int>(moves.size()));  // (10 - 2) * 4
    EXPECT_EQ(32, static_cast<int>(flags.size()));

    for (const auto& flag : flags) {
        EXPECT_EQ(1, flag);
    }

    for (const auto& move : moves) {
        EXPECT_EQ(printemps::neighborhood::MoveSense::Integer, move.sense);
        EXPECT_FALSE(move.alterations.front().first->is_fixed());

        EXPECT_EQ(1, static_cast<int>(move.alterations.size()));

        for (auto& constraint_ptr :
             move.alterations.front().first->related_constraint_ptrs()) {
            EXPECT_TRUE(move.related_constraint_ptrs.find(constraint_ptr) !=
                        move.related_constraint_ptrs.end());
        }
    }

    for (auto i = 0; i < 8; i++) {
        if (moves[4 * i].alterations.front().first->value() ==
            moves[4 * i].alterations.front().first->upper_bound()) {
            EXPECT_EQ(0, flags[4 * i]);
        } else {
            EXPECT_EQ(1, flags[4 * i]);
            EXPECT_EQ(moves[4 * i].alterations[0].second,
                      moves[4 * i].alterations[0].first->value() + 1);
        }

        if (moves[4 * i + 1].alterations.front().first->value() ==
            moves[4 * i + 1].alterations.front().first->lower_bound()) {
            EXPECT_EQ(0, flags[4 * i + 1]);
        } else {
            EXPECT_EQ(1, flags[4 * i + 1]);
            EXPECT_EQ(moves[4 * i + 1].alterations[0].second,
                      moves[4 * i + 1].alterations[0].first->value() - 1);
        }

        if (moves[4 * i + 2].alterations.front().first->value() ==
            moves[4 * i + 2].alterations.front().first->upper_bound()) {
            EXPECT_EQ(0, flags[4 * i + 2]);
        } else {
            EXPECT_EQ(1, flags[4 * i + 2]);
            EXPECT_EQ(moves[4 * i + 2].alterations[0].second,
                      (moves[4 * i + 2].alterations[0].first->value() +
                       moves[4 * i + 2].alterations[0].first->upper_bound()) /
                          2);
        }

        if (moves[4 * i + 3].alterations.front().first->value() ==
            moves[4 * i + 3].alterations.front().first->lower_bound()) {
            EXPECT_EQ(0, flags[4 * i + 3]);
        } else {
            EXPECT_EQ(1, flags[4 * i + 3]);
            EXPECT_EQ(moves[4 * i + 3].alterations[0].second,
                      (moves[4 * i + 3].alterations[0].first->value() +
                       moves[4 * i + 3].alterations[0].first->lower_bound()) /
                          2);
        }
    }
}  // namespace

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/