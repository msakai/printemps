/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_AGGREGATION_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_AGGREGATION_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class AggregationMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    AggregationMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    virtual ~AggregationMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(const std::vector<model::Constraint<T_Variable, T_Expression> *>
                   &a_RAW_CONSTRAINT_PTRS) {
        /**
         * Exclude constraints which contain fixed variables or selection
         * variables.
         */
        auto constraint_ptrs =
            extract_effective_constraint_ptrs(a_RAW_CONSTRAINT_PTRS);

        /**
         * Convert constraint objects to BinomialConstraint objects.
         */
        auto binomials = convert_to_binomial_constraints(constraint_ptrs);

        /**
         * Setup move objects.
         */
        const int BINOMIALS_SIZE = binomials.size();
        this->m_moves.resize(4 * BINOMIALS_SIZE);
        this->m_flags.resize(4 * BINOMIALS_SIZE);

        for (auto i = 0; i < BINOMIALS_SIZE; i++) {
            this->m_moves[4 * i].sense = MoveSense::Aggregation;
            this->m_moves[4 * i].alterations.emplace_back(
                binomials[i].variable_ptr_first, 0);
            this->m_moves[4 * i].alterations.emplace_back(
                binomials[i].variable_ptr_second, 0);
            this->m_moves[4 * i].is_univariable_move = false;

            utility::update_union_set(
                &(this->m_moves[4 * i].related_constraint_ptrs),
                binomials[i].variable_ptr_first->related_constraint_ptrs());

            utility::update_union_set(
                &(this->m_moves[4 * i].related_constraint_ptrs),
                binomials[i].variable_ptr_second->related_constraint_ptrs());

            this->m_moves[4 * i].is_special_neighborhood_move = true;
            this->m_moves[4 * i].is_available                 = true;
            this->m_moves[4 * i].overlap_rate                 = 0.0;

            this->m_moves[4 * i + 1] = this->m_moves[4 * i];
            this->m_moves[4 * i + 2] = this->m_moves[4 * i];
            this->m_moves[4 * i + 3] = this->m_moves[4 * i];
        }

        /**
         * Setup move updater.
         */
        auto move_updater =  //
            [this, binomials, BINOMIALS_SIZE](
                auto *                      a_moves,                          //
                auto *                      a_flags,                          //
                const bool                  a_ACCEPT_ALL,                     //
                const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < BINOMIALS_SIZE; i++) {
                    {
                        auto  index       = 4 * i;
                        auto &alterations = (*a_moves)[index].alterations;

                        alterations[0].second =
                            binomials[i].variable_ptr_first->value() + 1;
                        alterations[1].second =
                            static_cast<T_Variable>(std::floor(
                                (-binomials[i].constant_value -
                                 binomials[i].sensitivity_first *
                                     (binomials[i].variable_ptr_first->value() +
                                      1)) /
                                    binomials[i].sensitivity_second +
                                0.5));
                    }
                    {
                        auto  index       = 4 * i + 1;
                        auto &alterations = (*a_moves)[index].alterations;

                        alterations[0].second =
                            binomials[i].variable_ptr_first->value() - 1;
                        alterations[1].second =
                            static_cast<T_Variable>(std::floor(
                                (-binomials[i].constant_value -
                                 binomials[i].sensitivity_first *
                                     (binomials[i].variable_ptr_first->value() -
                                      1)) /
                                    binomials[i].sensitivity_second +
                                0.5));
                    }
                    {
                        auto  index       = 4 * i + 2;
                        auto &alterations = (*a_moves)[index].alterations;

                        alterations[0]
                            .second = static_cast<T_Variable>(std::floor(
                            (-binomials[i].constant_value -
                             binomials[i].sensitivity_second *
                                 (binomials[i].variable_ptr_second->value() +
                                  1)) /
                                binomials[i].sensitivity_first +
                            0.5));
                        alterations[1].second =
                            binomials[i].variable_ptr_second->value() + 1;
                    }
                    {
                        auto  index       = 4 * i + 3;
                        auto &alterations = (*a_moves)[index].alterations;

                        alterations[0]
                            .second = static_cast<T_Variable>(std::floor(
                            (-binomials[i].constant_value -
                             binomials[i].sensitivity_second *
                                 (binomials[i].variable_ptr_second->value() -
                                  1)) /
                                binomials[i].sensitivity_first +
                            0.5));
                        alterations[1].second =
                            binomials[i].variable_ptr_second->value() - 1;
                    }
                }
                const int MOVES_SIZE = a_moves->size();

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (!(*a_moves)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (neighborhood::has_fixed_variable((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (neighborhood::has_bound_violation((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            neighborhood::has_objective_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            neighborhood::has_feasibility_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        this->m_move_updater = move_updater;
    }
};
}  // namespace neighborhood
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/