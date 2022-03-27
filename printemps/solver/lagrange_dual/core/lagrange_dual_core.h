/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_H__

#include "lagrange_dual_core_termination_status.h"
#include "lagrange_dual_core_state.h"
#include "lagrange_dual_core_state_manager.h"
#include "lagrange_dual_core_result.h"

namespace printemps {
namespace solver {
namespace lagrange_dual {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LagrangeDualCore {
   private:
    model::Model<T_Variable, T_Expression>* m_model_ptr;

    std::vector<multi_array::ValueProxy<T_Variable>>
        m_initial_variable_value_proxies;

    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    option::Option                                       m_option;

    std::vector<solution::SparseSolution<T_Variable, T_Expression>>
        m_feasible_solutions;

    LagrangeDualCoreStateManager<T_Variable, T_Expression> m_state_manager;
    LagrangeDualCoreResult<T_Variable, T_Expression>       m_result;
    std::mt19937                                           m_get_rand_mt;

    /*************************************************************************/
    inline void preprocess(void) {
        /**
         * Reset the local augmented incumbent.
         */
        m_incumbent_holder_ptr->reset_local_augmented_incumbent();

        /**
         * Reset the feasible solutions storage.
         */
        m_feasible_solutions.clear();

        /**
         * Prepare a random generator, which is used for shuffling moves.
         */
        m_get_rand_mt.seed(m_option.local_search.seed);

        /**
         * Reset the last update iterations.
         */
        m_memory_ptr->reset_last_update_iterations();

        /**
         * Initialize the solution and update the model.
         */
        m_model_ptr->import_variable_values(m_initial_variable_value_proxies);
        m_model_ptr->update();

        m_state_manager.setup(m_model_ptr,             //
                              m_incumbent_holder_ptr,  //
                              m_memory_ptr,            //
                              m_option);
    }

    /*************************************************************************/
    inline void postprocess(void) {
        /**
         * Prepare the result.
         */
        m_result = LagrangeDualCoreResult<T_Variable, T_Expression>(
            m_state_manager.state());
    }

    /*************************************************************************/
    inline bool satisfy_time_over_terminate_condition(void) {
        const auto& STATE    = m_state_manager.state();
        const auto& m_option = this->m_option;

        if (STATE.elapsed_time > m_option.local_search.time_max) {
            m_state_manager.set_termination_status(
                LagrangeDualCoreTerminationStatus::TIME_OVER);
            return true;
        }

        if (STATE.elapsed_time + m_option.local_search.time_offset >
            m_option.time_max) {
            m_state_manager.set_termination_status(
                LagrangeDualCoreTerminationStatus::TIME_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_iteration_over_terminate_condition(void) {
        const auto& STATE    = m_state_manager.state();
        const auto& m_option = this->m_option;

        if (STATE.iteration >= m_option.local_search.iteration_max) {
            m_state_manager.set_termination_status(
                LagrangeDualCoreTerminationStatus::ITERATION_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_reach_target_terminate_condition(void) {
        auto        incumbent_holder_ptr = this->m_incumbent_holder_ptr;
        const auto& m_option             = this->m_option;

        if (incumbent_holder_ptr->feasible_incumbent_objective() <=
            m_option.target_objective_value) {
            m_state_manager.set_termination_status(
                LagrangeDualCoreTerminationStatus::REACH_TARGET);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_converge_terminate_condition(void) {
        const auto&  STATE         = m_state_manager.state();
        const auto&  m_option      = this->m_option;
        const double QUEUE_AVERAGE = STATE.queue.average();
        if (STATE.queue.size() == m_option.lagrange_dual.queue_size &&
            fabs(STATE.lagrangian - QUEUE_AVERAGE) <
                std::max(1.0, fabs(QUEUE_AVERAGE)) *
                    m_option.lagrange_dual.tolerance) {
            m_state_manager.set_termination_status(
                LagrangeDualCoreTerminationStatus::CONVERGE);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void print_table_header(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------+-----------+----------------------+--------"
            "--------------",
            true);
        utility::print(
            "Iteration| Lagrangian | Step Size |   Current Solution   |"
            "  Incumbent Solution ",
            true);
        utility::print(
            "         |            |           |   Aug.Obj.(Penalty)  | "
            "  Aug.Obj.  Feas.Obj ",
            true);
        utility::print(
            "---------+------------+-----------+----------------------+--------"
            "--------------",
            true);
    }

    /*************************************************************************/
    inline void print_table_initial(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        const auto& STATE = m_state_manager.state();
        const auto  SIGN  = m_model_ptr->sign();

        std::printf(  //
            " INITIAL |  %9.2e | %9.2e | %9.2e(%9.2e) | %9.2e  %9.2e\n",
            STATE.lagrangian * SIGN, STATE.step_size,
            STATE.current_solution_score.local_augmented_objective * SIGN,
            STATE.current_solution_score.is_feasible
                ? 0.0
                : STATE.current_solution_score.local_penalty,  //
            m_incumbent_holder_ptr->global_augmented_incumbent_objective() *
                SIGN,
            m_incumbent_holder_ptr->feasible_incumbent_objective() * SIGN);
    }

    /*************************************************************************/
    inline void print_table_body(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        const auto& STATE = m_state_manager.state();
        const auto  SIGN  = m_model_ptr->sign();

        char mark_current                    = ' ';
        char mark_global_augmented_incumbent = ' ';
        char mark_feasible_incumbent         = ' ';

        if (STATE.update_status & solution::IncumbentHolderConstant::
                                      STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current = '!';
        }

        if (STATE.update_status &
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current                    = '#';
            mark_global_augmented_incumbent = '#';
        }

        if (STATE.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            mark_current                    = '*';
            mark_global_augmented_incumbent = '*';
            mark_feasible_incumbent         = '*';
        }

        std::printf(  //
            "%8d |  %9.2e | %9.2e |%c%9.2e(%9.2e) |%c%9.2e %c%9.2e\n",
            STATE.iteration,                                                //
            STATE.lagrangian * SIGN,                                        //
            STATE.step_size,                                                //
            mark_current,                                                   //
            STATE.current_solution_score.local_augmented_objective * SIGN,  //
            STATE.current_solution_score.is_feasible
                ? 0.0
                : STATE.current_solution_score.local_penalty,  //
            mark_global_augmented_incumbent,                   //
            m_incumbent_holder_ptr->global_augmented_incumbent_objective() *
                SIGN,                 //
            mark_feasible_incumbent,  //
            m_incumbent_holder_ptr->feasible_incumbent_objective() * SIGN);
    }

    /*************************************************************************/
    inline void print_table_footer(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }
        utility::print(
            "---------+------------+-----------+----------------------+--------"
            "--------------",
            true);
    }

   public:
    /*************************************************************************/
    LagrangeDualCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualCore(model::Model<T_Variable, T_Expression>* a_model_ptr,     //
                     const std::vector<multi_array::ValueProxy<T_Variable>>&  //
                         a_INITIAL_VARIABLE_VALUE_PROXIES,                    //
                     solution::IncumbentHolder<T_Variable, T_Expression>*     //
                                                       a_incumbent_holder_ptr,  //
                     Memory<T_Variable, T_Expression>* a_memory_ptr,  //
                     const option::Option&             a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,                       //
                    a_INITIAL_VARIABLE_VALUE_PROXIES,  //
                    a_incumbent_holder_ptr,            //
                    a_memory_ptr,                      //
                    a_OPTION);
    }

    /*************************************************************************/
    virtual ~LagrangeDualCore(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
        m_initial_variable_value_proxies.clear();
        m_incumbent_holder_ptr = nullptr;
        m_memory_ptr           = nullptr;
        m_option.initialize();
        m_feasible_solutions.clear();

        m_state_manager.initialize();
        m_result.initialize();
        m_get_rand_mt.seed(0);
    }

    /*************************************************************************/
    inline void setup(                                             //
        model::Model<T_Variable, T_Expression>* a_model_ptr,       //
        const std::vector<multi_array::ValueProxy<T_Variable>>&    //
            a_INITIAL_VARIABLE_VALUE_PROXIES,                      //
        solution::IncumbentHolder<T_Variable, T_Expression>*       //
                                          a_incumbent_holder_ptr,  //
        Memory<T_Variable, T_Expression>* a_memory_ptr,            //
        const option::Option&             a_OPION) {
        m_model_ptr                      = a_model_ptr;
        m_initial_variable_value_proxies = a_INITIAL_VARIABLE_VALUE_PROXIES;
        m_incumbent_holder_ptr           = a_incumbent_holder_ptr;
        m_memory_ptr                     = a_memory_ptr;
        m_option                         = a_OPION;
        m_feasible_solutions.clear();
    }

    /*************************************************************************/
    inline void run(void) {
        /**
         * Start to measure computational time.
         */
        utility::TimeKeeper time_keeper;
        time_keeper.set_start_time();

        const auto& STATE = m_state_manager.state();

        /**
         * Preprocess.
         */
        this->preprocess();

        /**
         * Print the header of optimization progress table and print the initial
         * solution status.
         */
        utility::print_single_line(m_option.verbose >= option::verbose::Full);
        utility::print_message("Lagrange dual starts.",
                               m_option.verbose >= option::verbose::Full);
        print_table_header(m_option.verbose >= option::verbose::Full);
        print_table_initial(m_option.verbose >= option::verbose::Full);

        auto& variable_ptrs = m_model_ptr->variable_reference().variable_ptrs;
        const int VARIABLES_SIZE = variable_ptrs.size();

        /**
         * Iterations start.
         */
        m_state_manager.reset_iteration();

        while (true) {
            m_state_manager.set_elapsed_time(time_keeper.clock());
            /**
             * Terminate the loop if the time is over.
             */
            if (this->satisfy_time_over_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if the iteration is over.
             */
            if (this->satisfy_iteration_over_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if the objective value of the feasible
             * incumbent reaches the target value.
             */
            if (this->satisfy_reach_target_terminate_condition()) {
                break;
            }

            /**
             * Update the dual solution.
             */
            m_state_manager.update_dual();

            /**
             * Update the primal optimal solution so that it minimizes
             * lagrangian for the updated dual solution.
             */
            const double SIGN = m_model_ptr->sign();
#ifdef _OPENMP
#pragma omp parallel for if (m_option.is_enabled_parallel_evaluation) \
    schedule(static)
#endif
            for (auto i = 0; i < VARIABLES_SIZE; i++) {
                if (variable_ptrs[i]->is_fixed()) {
                    continue;
                }
                double coefficient = variable_ptrs[i]->objective_sensitivity();

                for (auto&& item :
                     variable_ptrs[i]->constraint_sensitivities()) {
                    const auto&  constraint_ptr = item.first;
                    const double SENSITIVITY    = item.second;

                    const int PROXY_INDEX = constraint_ptr->proxy_index();
                    const int FLAT_INDEX  = constraint_ptr->flat_index();

                    coefficient += STATE.dual[PROXY_INDEX].flat_indexed_values(
                                       FLAT_INDEX) *
                                   SENSITIVITY * SIGN;
                }

                variable_ptrs[i]->set_lagrangian_coefficient(coefficient);
                variable_ptrs[i]->set_lower_or_upper_bound(
                    (coefficient > 0) == (m_model_ptr->is_minimization()));
            }

            /**
             * Update the model.
             */
            m_model_ptr->update();

            /**
             * Update the state.
             */
            const auto SOLUTION_SCORE = m_model_ptr->evaluate({});
            m_state_manager.update(SOLUTION_SCORE);

            /**
             * Store the current feasible solution.
             */
            if (STATE.current_solution_score.is_feasible) {
                m_feasible_solutions.push_back(
                    m_model_ptr->export_plain_solution());
            }

            /**
             * Print the optimization progress.
             */
            if ((STATE.iteration %
                 std::max(m_option.lagrange_dual.log_interval, 1)) == 0 ||
                STATE.update_status > 1) {
                print_table_body(m_option.verbose >= option::verbose::Full);
            }

            /**
             * Terminate the loop if the step size of the subgradient algorithm
             * is small enough.
             */
            if (this->satisfy_converge_terminate_condition()) {
                break;
            }

            m_state_manager.next_iteration();
        }

        /**
         * Print the footer of the optimization progress table.
         */
        print_table_footer(m_option.verbose >= option::verbose::Full);

        /**
         * Postprocess.
         */
        this->postprocess();
    }

    /*************************************************************************/
    inline constexpr model::Model<T_Variable, T_Expression>* model_ptr(void) {
        return m_model_ptr;
    }

    /*************************************************************************/
    inline constexpr solution::IncumbentHolder<T_Variable, T_Expression>*
    incumbent_holder_ptr(void) {
        return m_incumbent_holder_ptr;
    }

    /*************************************************************************/
    inline constexpr Memory<T_Variable, T_Expression>* memory_ptr(void) {
        return m_memory_ptr;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        solution::SparseSolution<T_Variable, T_Expression>>&
    feasible_solutions(void) const {
        return m_feasible_solutions;
    }

    /*************************************************************************/
    inline constexpr const LagrangeDualCoreResult<T_Variable, T_Expression>&
    result(void) const {
        return m_result;
    }
};

}  // namespace core
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/