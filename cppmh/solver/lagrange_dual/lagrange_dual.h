/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_H__
#define CPPMH_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_H__

#include "lagrange_dual_option.h"
#include "lagrange_dual_print.h"
#include "lagrange_dual_result.h"

#include <deque>

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

/*****************************************************************************/
struct Option;

namespace lagrange_dual {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
void bound_dual(model::Model<T_Variable, T_Expression>* a_model,
                std::vector<model::ValueProxy<double>>* a_dual_value_proxies) {
    for (auto&& proxy : a_model->constraint_proxies()) {
        for (auto&& constraint : proxy.flat_indexed_constraints()) {
            int id         = constraint.id();
            int flat_index = constraint.flat_index();

            auto& lagrange_multiplier =
                (*a_dual_value_proxies)[id].flat_indexed_values(flat_index);

            switch (constraint.sense()) {
                case model::ConstraintSense::Lower: {
                    lagrange_multiplier = std::max(lagrange_multiplier, 0.0);
                    break;
                }
                case model::ConstraintSense::Upper: {
                    lagrange_multiplier = std::min(lagrange_multiplier, 0.0);
                    break;
                }
                case model::ConstraintSense::Equal: {
                    /// nothing to do
                    break;
                }
                default: {
                    /// nothing to do
                }
            }
        }
    }
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
LagrangeDualResult<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model,   //
    const Option&                           a_OPTION,  //
    const std::vector<model::ValueProxy<double>>&      //
        a_LOCAL_PENALTY_COEFFICIENT_PROXIES,           //
    const std::vector<model::ValueProxy<double>>&      //
        a_GLOBAL_PENALTY_COEFFICIENT_PROXIES,          //
    const std::vector<model::ValueProxy<T_Variable>>&  //
        a_INITIAL_VARIABLE_VALUE_PROXIES,              //
    const IncumbentHolder<T_Variable, T_Expression>&   //
        a_INCUMBENT_HOLDER) {
    /**
     * Define type aliases.
     */
    using Model_T           = model::Model<T_Variable, T_Expression>;
    using Result_T          = LagrangeDualResult<T_Variable, T_Expression>;
    using IncumbentHolder_T = IncumbentHolder<T_Variable, T_Expression>;

    /**
     * Start to measure computational time.
     */
    utility::TimeKeeper time_keeper;

    /**
     * Copy arguments as local variables.
     */
    Model_T* model  = a_model;
    Option   option = a_OPTION;

    std::vector<model::ValueProxy<double>> local_penalty_coefficient_proxies =
        a_LOCAL_PENALTY_COEFFICIENT_PROXIES;
    std::vector<model::ValueProxy<double>> global_penalty_coefficient_proxies =
        a_GLOBAL_PENALTY_COEFFICIENT_PROXIES;

    IncumbentHolder_T incumbent_holder = a_INCUMBENT_HOLDER;

    /**
     * Reset the local augmented incumbent.
     */
    incumbent_holder.reset_local_augmented_incumbent();

    model->import_variable_values(a_INITIAL_VARIABLE_VALUE_PROXIES);
    model->update();

    /**
     * Initialize the solution and update the model.
     */
    model::SolutionScore solution_score =
        model->evaluate({},                                 //
                        local_penalty_coefficient_proxies,  //
                        global_penalty_coefficient_proxies);

    int update_status =
        incumbent_holder.try_update_incumbent(model, solution_score);
    int total_update_status = 0;

    /**
     * Prepare the primal solution.
     */
    auto primal_incumbent = model->export_solution();

    /**
     * Prepare the dual solution as lagrange multipliers.
     */
    std::vector<model::ValueProxy<double>> dual_value_proxies =
        model->generate_constraint_parameter_proxies(0.0);
    bound_dual(model, &dual_value_proxies);

    auto dual_value_proxies_incumbent = dual_value_proxies;

    /**
     * Prepare the lagrangian incumbent and its queue.
     */
    double lagrangian_incumbent = -HUGE_VALF;

    utility::FixedSizeQueue<double> queue(option.lagrange_dual.queue_size);

    /**
     * Prepare the step size for subgradient algorithm.
     */
    double step_size = 1.0 / model->number_of_variables();

    /**
     * Print the header of optimization progress table and print the
     * initial solution status.
     */
    utility::print_single_line(option.verbose >= Verbose::Full);
    utility::print_message("Lagrange dual starts.",
                           option.verbose >= Verbose::Full);
    print_table_header(option.verbose >= Verbose::Full);
    print_table_initial(model, -HUGE_VALF, step_size, solution_score,
                        incumbent_holder, option.verbose >= Verbose::Full);

    /**
     * Iterations start.
     */
    int iteration = 0;

    while (true) {
        /**
         *  Check the terminating condition.
         */
        double elapsed_time = time_keeper.clock();
        if (elapsed_time > option.lagrange_dual.time_max) {
            break;
        }
        if (elapsed_time + option.lagrange_dual.time_offset > option.time_max) {
            break;
        }
        if (iteration >= option.lagrange_dual.iteration_max) {
            break;
        }

        /**
         * Update the dual solution.
         */
        for (auto&& proxy : a_model->constraint_proxies()) {
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                double constraint_value = constraint.constraint_value();
                int    id               = constraint.id();
                int    flat_index       = constraint.flat_index();

                dual_value_proxies[id].flat_indexed_values(flat_index) +=
                    step_size * constraint_value;
            }
        }

        /**
         * Bound the values of dual solution.
         */
        bound_dual(model, &dual_value_proxies);

        /**
         * Update the primal optimal solution so that it minimizes lagrangian
         * for the updated dual solution.
         */
        for (auto&& proxy : model->variable_proxies()) {
            for (auto&& variable : proxy.flat_indexed_variables()) {
                double coefficient = variable.objective_sensitivity();

                for (auto&& item : variable.constraint_sensitivities()) {
                    auto&  constraint  = item.first;
                    double sensitivity = item.second;

                    int id         = constraint->id();
                    int flat_index = constraint->flat_index();

                    coefficient +=
                        dual_value_proxies[id].flat_indexed_values(flat_index) *
                        sensitivity * model->sign();
                }

                if (coefficient > 0) {
                    if (model->is_minimization()) {
                        variable.set_value_if_not_fixed(variable.lower_bound());
                    } else {
                        variable.set_value_if_not_fixed(variable.upper_bound());
                    }

                } else {
                    if (model->is_minimization()) {
                        variable.set_value_if_not_fixed(variable.upper_bound());
                    } else {
                        variable.set_value_if_not_fixed(variable.lower_bound());
                    }
                }
            }
        }

        /**
         * Update the model.
         */
        model->update();
        solution_score = model->evaluate({},                                 //
                                         local_penalty_coefficient_proxies,  //
                                         global_penalty_coefficient_proxies);

        update_status =
            incumbent_holder.try_update_incumbent(model, solution_score);
        total_update_status = update_status || total_update_status;

        /**
         * Compute the lagrangian value.
         */
        double lagrangian =
            model->compute_lagrangian(dual_value_proxies) * model->sign();

        /**
         * Update the lagrangian incumbent.
         */
        if (lagrangian > lagrangian_incumbent) {
            lagrangian_incumbent         = lagrangian;
            auto primal_incumbent        = model->export_solution();
            dual_value_proxies_incumbent = dual_value_proxies;
        }

        /**
         * Update the lagrangian queue.
         */
        queue.push(lagrangian);
        double queue_average = queue.average();
        double queue_max     = queue.max();

        /**
         * Adjust the step size.
         */
        if (queue.size() > 0) {
            if (lagrangian > queue_average) {
                step_size *= option.lagrange_dual.step_size_extend_rate;
            }
            if (lagrangian < queue_max) {
                step_size *= option.lagrange_dual.step_size_reduce_rate;
            }
        }

        /**
         * Print the optimization progress.
         */
        if (iteration % std::max(option.lagrange_dual.log_interval, 1) == 0 ||
            update_status > 1) {
            print_table_body(model,             //
                             iteration,         //
                             lagrangian,        //
                             step_size,         //
                             solution_score,    //
                             update_status,     //
                             incumbent_holder,  //
                             option.verbose >= Verbose::Full);
        }

        /**
         * Terminate the loop if lagrangian converges.
         */
        if (queue.size() > option.lagrange_dual.queue_size &&
            (lagrangian_incumbent - queue_average) <
                std::max(1.0, fabs(queue_average)) *
                    option.lagrange_dual.tolerance) {
            break;
        }

        iteration++;
    }

    /**
     * Print the footer of the optimization progress table.
     */
    print_table_footer(option.verbose >= Verbose::Full);

    /**
     * Prepare the result.
     */
    Result_T result;
    result.lagrangian           = lagrangian_incumbent;
    result.primal_solution      = primal_incumbent;
    result.dual_value_proxies   = dual_value_proxies_incumbent;
    result.incumbent_holder     = incumbent_holder;
    result.total_update_status  = total_update_status;
    result.number_of_iterations = iteration;

    return result;
}
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/