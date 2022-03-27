/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_H__

#include "local_search_core_termination_status.h"
#include "local_search_core_state.h"
#include "local_search_core_state_manager.h"
#include "local_search_core_result.h"

namespace printemps {
namespace solver {
namespace local_search {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LocalSearchCore {
   private:
    model::Model<T_Variable, T_Expression>* m_model_ptr;

    std::vector<multi_array::ValueProxy<T_Variable>>
        m_initial_variable_value_proxies;

    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    option::Option                                       m_option;

    std::vector<solution::SparseSolution<T_Variable, T_Expression>>
        m_feasible_solutions;

    LocalSearchCoreStateManager<T_Variable, T_Expression> m_state_manager;
    LocalSearchCoreResult<T_Variable, T_Expression>       m_result;
    std::mt19937                                          m_get_rand_mt;

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

        /**
         * Reset the variable improvability.
         */
        m_model_ptr->reset_variable_objective_improvabilities();
        m_model_ptr->reset_variable_feasibility_improvabilities();

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
        m_result = LocalSearchCoreResult<T_Variable, T_Expression>(
            m_state_manager.state());
    }

    /*************************************************************************/
    inline bool satisfy_time_over_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.elapsed_time > m_option.local_search.time_max) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::TIME_OVER);
            return true;
        }

        if (STATE.elapsed_time + m_option.local_search.time_offset >
            m_option.time_max) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::TIME_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_iteration_over_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.iteration >= m_option.local_search.iteration_max) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::ITERATION_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_reach_target_terminate_condition(void) {
        if (m_incumbent_holder_ptr->feasible_incumbent_objective() <=
            m_option.target_objective_value) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::REACH_TARGET);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_optimal_or_no_move_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.number_of_moves > 0) {
            return false;
        }

        if (m_model_ptr->is_linear() && m_model_ptr->is_feasible()) {
            /**
             * NOTE: If the current solution is feasible and there is no
             * improvable solution, the solution should be an optimum. It can
             * happen for decomp2 instance in MIPLIB 2017.
             */
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::OPTIMAL);
            for (const auto& variable_ptr :
                 m_model_ptr->variable_reference().variable_ptrs) {
                if (variable_ptr->is_objective_improvable()) {
                    m_state_manager.set_termination_status(
                        LocalSearchCoreTerminationStatus::NO_MOVE);
                    break;
                }
            }
            return true;
        } else {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::NO_MOVE);
            return true;
        }

        return false;
    }

    /*************************************************************************/
    inline bool satisfy_local_optimal_terminate_condition(
        const bool a_IS_FOUND_IMPROVING_SOLUTION) {
        if (!a_IS_FOUND_IMPROVING_SOLUTION) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::LOCAL_OPTIMAL);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void update_moves() {
        const auto& STATE = m_state_manager.state();

        bool accept_all                    = true;
        bool accept_objective_improvable   = true;
        bool accept_feasibility_improvable = true;

        if (m_model_ptr->is_linear()) {
            m_model_ptr->neighborhood().update_moves(
                accept_all,                     //
                accept_objective_improvable,    //
                accept_feasibility_improvable,  //
                m_option.is_enabled_parallel_neighborhood_update);

            m_state_manager.set_number_of_moves(
                m_model_ptr->neighborhood().move_ptrs().size());
            return;
        }

        if (STATE.iteration == 0) {
            m_model_ptr->update_variable_objective_improvabilities();
        } else {
            m_model_ptr->update_variable_objective_improvabilities(
                utility::to_vector(
                    neighborhood::related_variable_ptrs(STATE.current_move)));
        }

        if (m_model_ptr->is_feasible()) {
            accept_all                    = false;
            accept_objective_improvable   = true;
            accept_feasibility_improvable = false;
        } else {
            m_model_ptr->reset_variable_feasibility_improvabilities();
            m_model_ptr->update_variable_feasibility_improvabilities();

            accept_all                    = false;
            accept_objective_improvable   = false;
            accept_feasibility_improvable = true;
        }

        m_model_ptr->neighborhood().update_moves(
            accept_all,                     //
            accept_objective_improvable,    //
            accept_feasibility_improvable,  //
            m_option.is_enabled_parallel_neighborhood_update);

        m_state_manager.set_number_of_moves(
            m_model_ptr->neighborhood().move_ptrs().size());
    }

    /*************************************************************************/
    inline void update_memory(
        const neighborhood::Move<T_Variable, T_Expression>* a_move_ptr) {
        const auto& STATE = m_state_manager.state();
        m_memory_ptr->update(*a_move_ptr, STATE.iteration);
    }

    /*************************************************************************/
    inline void print_table_header(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
        utility::print(
            "Iteration| Number of Neighborhoods|   Current Solution   |"
            "  Incumbent Solution ",
            true);
        utility::print(
            "         |      All       checked |   Aug.Obj.(Penalty)  | "
            "  Aug.Obj.  Feas.Obj ",
            true);
        utility::print(
            "---------+------------------------+----------------------+--------"
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

        std::printf(
            " INITIAL |          -           - | %9.2e(%9.2e) | %9.2e  %9.2e\n",
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

        if (STATE.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current = '!';
        }

        if (STATE.update_status &  //
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
            "%8d |      %5d       %5d |%c%9.2e(%9.2e) |%c%9.2e %c%9.2e\n",
            STATE.iteration,                                                //
            STATE.number_of_moves,                                          //
            STATE.number_of_checked_moves,                                  //
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
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
    }

   public:
    /*************************************************************************/
    LocalSearchCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchCore(model::Model<T_Variable, T_Expression>* a_model_ptr,     //
                    const std::vector<multi_array::ValueProxy<T_Variable>>&  //
                        a_INITIAL_VARIABLE_VALUE_PROXIES,                    //
                    solution::IncumbentHolder<T_Variable, T_Expression>*     //
                                                      a_incumbent_holder_ptr,  //
                    Memory<T_Variable, T_Expression>* a_memory_ptr,  //
                    const option::Option&             a_OPION) {
        this->initialize();
        this->setup(a_model_ptr,                       //
                    a_INITIAL_VARIABLE_VALUE_PROXIES,  //
                    a_incumbent_holder_ptr,            //
                    a_memory_ptr,                      //
                    a_OPION);
    }

    /*************************************************************************/
    virtual ~LocalSearchCore(void) {
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
        utility::print_message("Local search starts.",
                               m_option.verbose >= option::verbose::Full);
        this->print_table_header(m_option.verbose >= option::verbose::Full);
        this->print_table_initial(m_option.verbose >= option::verbose::Full);

        /**
         * Iterations start.
         */
        m_state_manager.reset_iteration();
        solution::SolutionScore trial_solution_score;
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
             * Update the moves.
             */
            this->update_moves();

            const auto& MOVE_PTRS = m_model_ptr->neighborhood().move_ptrs();
            m_state_manager.set_number_of_moves(MOVE_PTRS.size());

            /**
             *  Check the terminating condition (2).
             */
            if (this->satisfy_optimal_or_no_move_terminate_condition()) {
                break;
            }

            int  number_of_checked_move      = 0;
            bool is_found_improving_solution = false;
            neighborhood::Move<T_Variable, T_Expression>* selected_move_ptr =
                nullptr;

            const auto CURRENT_SOLUTION_SCORE = STATE.current_solution_score;
            const auto CURRENT_LOCAL_AUGMENTED_OBJECTIVE =
                CURRENT_SOLUTION_SCORE.local_augmented_objective;
            for (const auto& move_ptr : MOVE_PTRS) {
                /**
                 * The neighborhood solutions are evaluated in sequential by
                 * fast or ordinary(slow) evaluation methods.
                 */
#ifndef _MPS_SOLVER
                if (m_model_ptr->is_enabled_fast_evaluation()) {
#endif
                    if (move_ptr->is_univariable_move) {
                        m_model_ptr->evaluate_single(&trial_solution_score,  //
                                                     *move_ptr,              //
                                                     CURRENT_SOLUTION_SCORE);
                    } else {
                        m_model_ptr->evaluate_multi(&trial_solution_score,  //
                                                    *move_ptr,              //
                                                    CURRENT_SOLUTION_SCORE);
                    }
#ifndef _MPS_SOLVER
                } else {
                    m_model_ptr->evaluate(&trial_solution_score, *move_ptr);
                }
#endif
                /**
                 * Update the incumbent if the evaluated solution improves it.
                 */
                if (trial_solution_score.local_augmented_objective +
                        constant::EPSILON <
                    CURRENT_LOCAL_AUGMENTED_OBJECTIVE) {
                    selected_move_ptr           = move_ptr;
                    is_found_improving_solution = true;
                    break;
                }

                number_of_checked_move++;
            }

            /**
             * Terminate the loop if there is no improving solution in the
             * checked neighborhood.
             */
            if (this->satisfy_local_optimal_terminate_condition(
                    is_found_improving_solution)) {
                break;
            }

            /**
             * Update the model by the selected move.
             */
            m_model_ptr->update(*selected_move_ptr);

            /**
             * Update the memory.
             */
            this->update_memory(selected_move_ptr);

            /**
             * Update the state.
             */
            m_state_manager.update(selected_move_ptr,            //
                                   number_of_checked_move,       //
                                   is_found_improving_solution,  //
                                   trial_solution_score);

            /**
             * Store the current feasible solution.
             */
            if (STATE.current_solution_score.is_feasible) {
                m_feasible_solutions.push_back(
                    m_model_ptr->export_sparse_solution());
            }

            /**
             * Print the optimization progress.
             */
            if ((STATE.iteration %
                 std::max(m_option.local_search.log_interval, 1)) == 0 ||
                STATE.update_status > 1) {
                this->print_table_body(m_option.verbose >=
                                       option::verbose::Full);
            }

            m_state_manager.next_iteration();
        }

        /**
         * Print the footer of the optimization progress table.
         */
        this->print_table_footer(m_option.verbose >= option::verbose::Full);

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
    inline constexpr const LocalSearchCoreResult<T_Variable, T_Expression>&
    result(void) const {
        return m_result;
    }
};
}  // namespace core
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/