/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_MODEL_H__
#define CPPMH_MODEL_MODEL_H__

#include <vector>
#include <string>
#include <numeric>
#include <functional>

#include "range.h"
#include "move.h"
#include "variable_proxy.h"
#include "expression_proxy.h"
#include "constraint_proxy.h"
#include "objective.h"
#include "value_proxy.h"
#include "solution.h"
#include "solution_score.h"
#include "neighborhood.h"

#include "expression_binary_operator.h"
#include "constraint_binary_operator.h"

namespace cppmh {
namespace model {
/*****************************************************************************/
struct ModelConstant {
    /**
     * Since the addresses of created variables, expressions, and constraints
     * must not be reallocated, addresses for them are reserved beforehand, and
     * the number of their defintions cannot  exceed the following limits.
     */
    static constexpr int MAX_NUMBER_OF_VARIABLE_PROXIES   = 100;
    static constexpr int MAX_NUMBER_OF_EXPRESSION_PROXIES = 100;
    static constexpr int MAX_NUMBER_OF_CONSTRAINT_PROXIES = 100;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model {
   private:
    std::vector<VariableProxy<T_Variable, T_Expression>>   m_variable_proxies;
    std::vector<ExpressionProxy<T_Variable, T_Expression>> m_expression_proxies;
    std::vector<ConstraintProxy<T_Variable, T_Expression>> m_constraint_proxies;

    Objective<T_Variable, T_Expression> m_objective;

    std::vector<std::string> m_variable_names;
    std::vector<std::string> m_expression_names;
    std::vector<std::string> m_constraint_names;

    bool                                   m_is_defined_objective;
    bool                                   m_is_minimization;
    Neighborhood<T_Variable, T_Expression> m_neighborhood;
    std::function<void(void)>              m_callback;

   public:
    /*************************************************************************/
    Model(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~Model(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_variable_proxies.reserve(
            ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES);
        m_expression_proxies.reserve(
            ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES);
        m_constraint_proxies.reserve(
            ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES);
        m_objective.initialize();

        m_variable_names.clear();
        m_expression_names.clear();
        m_constraint_names.clear();

        m_is_defined_objective = false;
        m_is_minimization      = true;
        m_neighborhood.initialize();
        m_callback = [](void) {};
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &create_variable(
        const std::string &a_NAME) {
        int id = m_variable_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        m_variable_proxies.emplace_back(
            VariableProxy<T_Variable, T_Expression>::create_instance(id));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &create_variable(
        const std::string &a_NAME, const T_Variable a_LOWER_BOUND,
        const T_Variable a_UPPER_BOUND) {
        auto &variable_proxy = create_variable(a_NAME);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME, const int a_NUMBER_OF_ELEMENTS) {
        int id = m_variable_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        m_variable_proxies.emplace_back(
            VariableProxy<T_Variable, T_Expression>::create_instance(
                id, a_NUMBER_OF_ELEMENTS));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME, const int a_NUMBER_OF_ELEMENTS,
        const T_Variable a_LOWER_BOUND, const T_Variable a_UPPER_BOUND) {
        auto &variable_proxy = create_variables(a_NAME, a_NUMBER_OF_ELEMENTS);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME, const std::vector<int> &a_SHAPE) {
        int id = m_variable_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        m_variable_proxies.emplace_back(
            VariableProxy<T_Variable, T_Expression>::create_instance(id,
                                                                     a_SHAPE));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME, const std::vector<int> &a_SHAPE,
        const T_Variable a_LOWER_BOUND, const T_Variable a_UPPER_BOUND) {
        auto &variable_proxy = create_variables(a_NAME, a_SHAPE);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ExpressionProxy<T_Variable, T_Expression>
        &create_expression(const std::string &a_NAME) {
        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(id));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ExpressionProxy<T_Variable, T_Expression> &
    create_expressions(const std::string &a_NAME, int a_NUMBER_OF_ELEMENTS) {
        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(
                id, a_NUMBER_OF_ELEMENTS));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ExpressionProxy<T_Variable, T_Expression>
        &create_expressions(const std::string &     a_NAME,
                            const std::vector<int> &a_SHAPE) {
        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(
                id, a_SHAPE));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr ExpressionProxy<T_Variable, T_Expression> &
    create_expression(
        const std::string &                               a_NAME,
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(id));
        m_expression_names.push_back(a_NAME);
        m_expression_proxies.back() = a_EXPRESSION_LIKE.to_expression();
        return m_expression_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ExpressionProxy<T_Variable, T_Expression>
        &create_expression(
            const std::string &                         a_NAME,
            const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(id));
        m_expression_names.push_back(a_NAME);
        m_expression_proxies.back() = a_EXPRESSION;
        return m_expression_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ConstraintProxy<T_Variable, T_Expression>
        &create_constraint(const std::string &a_NAME) {
        int id = m_constraint_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            ConstraintProxy<T_Variable, T_Expression>::create_instance(id));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ConstraintProxy<T_Variable, T_Expression> &
    create_constraints(const std::string &a_NAME, int a_NUMBER_OF_ELEMENTS) {
        int id = m_constraint_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            ConstraintProxy<T_Variable, T_Expression>::create_instance(
                id, a_NUMBER_OF_ELEMENTS));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ConstraintProxy<T_Variable, T_Expression>
        &create_constraints(const std::string &     a_NAME,
                            const std::vector<int> &a_SHAPE) {
        int id = m_constraint_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            ConstraintProxy<T_Variable, T_Expression>::create_instance(
                id, a_SHAPE));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ConstraintProxy<T_Variable, T_Expression>
        &create_constraint(
            const std::string &                         a_NAME,
            const Constraint<T_Variable, T_Expression> &a_CONSTRAINT) {
        int id = m_constraint_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            ConstraintProxy<T_Variable, T_Expression>::create_instance(id));
        m_constraint_names.push_back(a_NAME);
        m_constraint_proxies.back() = a_CONSTRAINT;

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline constexpr void minimize(
        const std::function<
            T_Expression(const Move<T_Variable, T_Expression> &)> &a_FUNCTION) {
        auto objective =
            Objective<T_Variable, T_Expression>::create_instance(a_FUNCTION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = true;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr void minimize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        auto objective = Objective<T_Variable, T_Expression>::create_instance(
            a_EXPRESSION_LIKE.to_expression());

        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = true;
    }

    /*************************************************************************/
    inline constexpr void minimize(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        auto objective =
            Objective<T_Variable, T_Expression>::create_instance(a_EXPRESSION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = true;
    }

    /*************************************************************************/
    inline constexpr void maximize(
        const std::function<
            T_Expression(const Move<T_Variable, T_Expression> &)> &a_FUNCTION) {
        auto objective =
            Objective<T_Variable, T_Expression>::create_instance(a_FUNCTION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = false;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr void maximize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        auto objective = Objective<T_Variable, T_Expression>::create_instance(
            a_EXPRESSION_LIKE.to_expression());
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = false;
    }

    /*************************************************************************/
    inline constexpr void maximize(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        auto objective =
            Objective<T_Variable, T_Expression>::create_instance(a_EXPRESSION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = false;
    }

    /*************************************************************************/
    inline constexpr bool is_defined_objective(void) const {
        return m_is_defined_objective;
    }

    /*************************************************************************/
    inline constexpr bool is_minimization(void) const {
        return m_is_minimization;
    }

    /*************************************************************************/
    inline constexpr double sign(void) const {
        /**
         * In this program, maximization problems are solved as minimization
         * problems by nagating the objective function values. This method is
         * used to show objective function values for output.
         */
        return m_is_minimization ? 1.0 : -1.0;
    }

    /*************************************************************************/
    inline constexpr int number_of_variables(void) const {
        int result = 0;
        for (auto &&proxy : m_variable_proxies) {
            result += proxy.number_of_elements();
        }
        return result;
    }

    /*************************************************************************/
    inline constexpr Neighborhood<T_Variable, T_Expression> &neighborhood(
        void) {
        return m_neighborhood;
    }

    /*************************************************************************/
    inline constexpr void setup_default_neighborhood(
        const bool a_IS_ENABLED_PARALLEL) {
        m_neighborhood.setup_default_neighborhood(
            &m_variable_proxies, &m_constraint_proxies, a_IS_ENABLED_PARALLEL);
    }

    /*************************************************************************/
    inline constexpr void setup_has_fixed_variables(void) {
        m_neighborhood.setup_has_fixed_variables(m_variable_proxies);
    }

    /*************************************************************************/
    inline constexpr void setup_fixed_sensitivities(void) {
        for (auto &&proxy : m_expression_proxies) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                expression.setup_fixed_sensitivities();
            }
        }
    }

    /*************************************************************************/
    inline constexpr void reset_variable_sense(void) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.reset_sense();
            }
        }
    }

    /*************************************************************************/
    inline constexpr void verify_problem(void) {
        if (m_variable_proxies.size() == 0) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "No decision variables are defined."));
        }
        if (m_constraint_proxies.size() == 0 && !m_is_defined_objective) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Neither objective nor constraint functions are defined."));
        }
    }

    /*************************************************************************/
    inline constexpr void verify_bounds(void) {
        /// This method is not used
        for (const auto &proxy : m_variable_proxies) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                if (!variable.is_defined_bounds()) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There are one or more unbounded variables."));
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void verify_and_correct_selection_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTON, const bool a_IS_ENABLED_PRINT) {
        for (auto &&selection : m_neighborhood.selections()) {
            std::vector<Variable<T_Variable, T_Expression> *>
                fixed_selected_variable_ptrs;
            std::vector<Variable<T_Variable, T_Expression> *>
                selected_variable_ptrs;
            std::vector<Variable<T_Variable, T_Expression> *>
                fixed_invalid_variable_ptrs;
            std::vector<Variable<T_Variable, T_Expression> *>
                invalid_variable_ptrs;

            for (auto &&variable_ptr : selection.variable_ptrs) {
                if (variable_ptr->value() == 1) {
                    selected_variable_ptrs.push_back(variable_ptr);
                    if (variable_ptr->is_fixed()) {
                        fixed_selected_variable_ptrs.push_back(variable_ptr);
                    }
                }
                if (variable_ptr->value() != 0 && variable_ptr->value() != 1) {
                    invalid_variable_ptrs.push_back(variable_ptr);
                    if (variable_ptr->is_fixed()) {
                        fixed_invalid_variable_ptrs.push_back(variable_ptr);
                    }
                }
            }

            /**
             * Return logic error if there is an invalid fixed variable.
             */
            if (fixed_invalid_variable_ptrs.size() > 0) {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "There is an invalid fixed variable."));
            }

            /**
             * Return logic error if there are more than 1 fixed selected
             * variables.
             */
            if (fixed_selected_variable_ptrs.size() > 1) {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "There are more than one fixed selected variables."));
            }

            /**
             * Correct initial values or return logic error if there is a
             * variable of which initial value violates binary constraint.
             */
            if (invalid_variable_ptrs.size() > 0) {
                if (a_IS_ENABLED_CORRECTON) {
                    for (auto &&variable_ptr : invalid_variable_ptrs) {
                        auto old_value = variable_ptr->value();
                        auto new_value = 0;

                        variable_ptr->set_value_if_not_fixed(new_value);

                        auto label =
                            m_variable_names[variable_ptr->id()] +
                            m_variable_proxies[variable_ptr->id()]
                                .indices_label(variable_ptr->flat_index());
                        utility::print_warning(
                            "The initial value " + label + " = " +
                                std::to_string(old_value) +
                                " is corrected automatically as " + label +
                                " = " + std::to_string(new_value) +
                                " to satisfy binary constraint.",
                            a_IS_ENABLED_PRINT);
                    }

                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There is a variable of which initial value violates "
                        "binary constraint."));
                }
            }

            /**
             * Correct initial values or return logic error if there are more
             * than 1 selected variables.
             */
            if (selected_variable_ptrs.size() > 1) {
                if (a_IS_ENABLED_CORRECTON) {
                    Variable<T_Variable, T_Expression> *selected_variable_ptr =
                        nullptr;
                    if (fixed_selected_variable_ptrs.size() == 1) {
                        selected_variable_ptr =
                            fixed_selected_variable_ptrs.front();
                    } else {
                        selected_variable_ptr = selected_variable_ptrs.front();
                    }

                    for (auto &&variable_ptr : selected_variable_ptrs) {
                        if (variable_ptr != selected_variable_ptr) {
                            auto old_value = 1;
                            auto new_value = 0;

                            variable_ptr->set_value_if_not_fixed(new_value);

                            auto label =
                                m_variable_names[variable_ptr->id()] +
                                m_variable_proxies[variable_ptr->id()]
                                    .indices_label(variable_ptr->flat_index());
                            utility::print_warning(
                                "The initial value " + label + " = " +
                                    std::to_string(old_value) +
                                    " is corrected automatically as " + label +
                                    " = " + std::to_string(new_value) +
                                    " to satisfy binary constraint.",
                                a_IS_ENABLED_PRINT);
                        }
                    }

                    selected_variable_ptr->set_value_if_not_fixed(1);
                    selected_variable_ptr->select();

                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There are more than one selected variables."));
                }
            }
            /**
             * Correct initial values or return logic error if there is no
             * selected variables.
             */
            else if (selected_variable_ptrs.size() == 0) {
                if (a_IS_ENABLED_CORRECTON) {
                    auto old_value    = 0;
                    auto new_value    = 1;
                    bool is_corrected = false;
                    for (auto &&variable_ptr : selection.variable_ptrs) {
                        if (!variable_ptr->is_fixed()) {
                            variable_ptr->set_value_if_not_fixed(new_value);

                            auto label =
                                m_variable_names[variable_ptr->id()] +
                                m_variable_proxies[variable_ptr->id()]
                                    .indices_label(variable_ptr->flat_index());
                            utility::print_warning(
                                "The initial value " + label + " = " +
                                    std::to_string(old_value) +
                                    " is corrected automatically as " + label +
                                    " = " + std::to_string(new_value) +
                                    " to satisfy binary constraint.",
                                a_IS_ENABLED_PRINT);
                            is_corrected = true;
                            break;
                        }
                    }
                    if (!is_corrected) {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The initial value could not be modified because "
                            "all variables are fixed."));
                    };
                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There is no selected variables."));
                }
            } else {
                selected_variable_ptrs.front()->select();
            }
        }
    }

    /*************************************************************************/
    inline constexpr void verify_and_correct_binary_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTON, const bool a_IS_ENABLED_PRINT) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (variable.sense() == VariableSense::Binary) {
                    if (variable.value() != 0 && variable.value() != 1) {
                        if (variable.is_fixed()) {
                            throw std::logic_error(
                                utility::format_error_location(
                                    __FILE__, __LINE__, __func__,
                                    "There is an invalid fixed variable."));
                        }

                        if (a_IS_ENABLED_CORRECTON) {
                            T_Variable old_value = variable.value();
                            T_Variable new_value = variable.value();
                            if (variable.value() < variable.lower_bound()) {
                                new_value = variable.lower_bound();
                            } else if (variable.value() >
                                       variable.upper_bound()) {
                                new_value = variable.upper_bound();
                            }

                            variable.set_value_if_not_fixed(new_value);

                            auto label =
                                m_variable_names[variable.id()] +
                                proxy.indices_label(variable.flat_index());
                            utility::print_warning(
                                "The initial value " + label + " = " +
                                    std::to_string(old_value) +
                                    " is corrected automatically as " + label +
                                    " = " + std::to_string(new_value) +
                                    " to satisfy binary constraint.",
                                a_IS_ENABLED_PRINT);
                        } else {
                            throw std::logic_error(
                                utility::format_error_location(
                                    __FILE__, __LINE__, __func__,
                                    "An initial value violates binary "
                                    "constraint."));
                        }
                    }
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void verify_and_correct_integer_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTON, const bool a_IS_ENABLED_PRINT) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (variable.sense() == VariableSense::Integer &&
                    (variable.value() < variable.lower_bound() ||
                     variable.value() > variable.upper_bound())) {
                    if (variable.is_fixed()) {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "There is an invalid fixed variable"));
                    }

                    if (a_IS_ENABLED_CORRECTON) {
                        T_Variable old_value = variable.value();
                        T_Variable new_value = variable.value();
                        if (variable.value() < variable.lower_bound()) {
                            new_value = variable.lower_bound();
                        } else if (variable.value() > variable.upper_bound()) {
                            new_value = variable.upper_bound();
                        }

                        variable.set_value_if_not_fixed(new_value);

                        auto label = m_variable_names[variable.id()] +
                                     proxy.indices_label(variable.flat_index());
                        utility::print_warning(
                            "The initial value " + label + " = " +
                                std::to_string(old_value) +
                                " is corrected automatically as " + label +
                                " = " + std::to_string(new_value) +
                                " to satisfy lower or upper bound constraint.",
                            a_IS_ENABLED_PRINT);
                    } else {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "An initial value violates the lower or upper "
                            "bound constraint."));
                    }
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void set_callback(
        const std::function<void(void)> &a_CALLBACK) {
        m_callback = a_CALLBACK;
    }

    /*************************************************************************/
    inline constexpr void callback(void) {
        m_callback();
    }

    /*************************************************************************/
    inline constexpr void import_variable_values(
        const std::vector<ValueProxy<T_Variable>> &a_PROXIES) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                int id         = variable.id();
                int flat_index = variable.flat_index();
                variable.set_value_if_not_fixed(
                    a_PROXIES[id].flat_indexed_values(flat_index));
            }
        }
        this->verify_and_correct_selection_variables_initial_values(false,
                                                                    false);
        this->verify_and_correct_integer_variables_initial_values(false, false);
        this->verify_and_correct_integer_variables_initial_values(false, false);
    }

    /*************************************************************************/
    inline constexpr void update(void) {
        /**
         * Update in order of expressions -> objective, constraints. For
         * typical problem.
         */
        for (auto &&expression_proxy : m_expression_proxies) {
            for (auto &&expression :
                 expression_proxy.flat_indexed_expressions()) {
                expression.update();
            }
        }

        for (auto &&constraint_proxy : m_constraint_proxies) {
            for (auto &&constraint :
                 constraint_proxy.flat_indexed_constraints()) {
                constraint.update();
            }
        }

        m_objective.update();
    }

    /*************************************************************************/
    inline constexpr void update(const Move<T_Variable, T_Expression> &a_MOVE) {
        /**
         * Update in order of objective, constraints -> expressions ->
         * variables.
         */
        m_objective.update(a_MOVE);

        for (auto &&constraint_proxy : m_constraint_proxies) {
            for (auto &&constraint :
                 constraint_proxy.flat_indexed_constraints()) {
                if (constraint.is_enabled()) {
                    constraint.update(a_MOVE);
                }
            }
        }

        for (auto &&expression_proxy : m_expression_proxies) {
            for (auto &&expression :
                 expression_proxy.flat_indexed_expressions()) {
                if (expression.is_enabled()) {
                    expression.update(a_MOVE);
                }
            }
        }

        for (auto &&alteration : a_MOVE.alterations) {
            alteration.first->set_value_if_not_fixed(alteration.second);
        }

        if (a_MOVE.sense == MoveSense::Selection) {
            a_MOVE.alterations[1].first->select();
        }
    }

    /*************************************************************************/
    inline SolutionScore evaluate(
        const Move<T_Variable, T_Expression> &a_MOVE,
        const std::vector<ValueProxy<double>>
            &a_LOCAL_PENALTY_COEFFICIENT_PROXIES,
        const std::vector<ValueProxy<double>>
            &a_GLOBAL_PENALTY_COEFFICIENT_PROXIES) noexcept {
        double local_penalty  = 0.0;
        double global_penalty = 0.0;

        std::size_t number_of_constraint_proxies = m_constraint_proxies.size();
        bool        is_constraint_improvable     = false;
        bool        is_feasible                  = true;

        for (std::size_t i = 0; i < number_of_constraint_proxies; i++) {
            auto &constraints =
                m_constraint_proxies[i].flat_indexed_constraints();
            int number_of_constraint = constraints.size();

            for (auto j = 0; j < number_of_constraint; j++) {
                if (!constraints[j].is_enabled()) {
                    continue;
                }
                auto violation = constraints[j].evaluate_violation(a_MOVE);
                if (violation < constraints[j].violation_value()) {
                    is_constraint_improvable = true;
                }

                if (violation > 0) {
                    is_feasible = false;
                }

                local_penalty +=
                    a_LOCAL_PENALTY_COEFFICIENT_PROXIES[i].flat_indexed_values(
                        j) *
                    violation;
                global_penalty +=
                    a_GLOBAL_PENALTY_COEFFICIENT_PROXIES[i].flat_indexed_values(
                        j) *
                    violation;
            }
        }

        double objective             = m_objective.evaluate(a_MOVE);
        double objective_improvement = m_objective.value() - objective;

        objective *= this->sign();
        objective_improvement *= this->sign();

        double local_augmented_objective  = objective + local_penalty;
        double global_augmented_objective = objective + global_penalty;

        SolutionScore score;

        score.objective                  = objective;
        score.objective_improvement      = objective_improvement;
        score.local_penalty              = local_penalty;
        score.global_penalty             = global_penalty;
        score.local_augmented_objective  = local_augmented_objective;
        score.global_augmented_objective = global_augmented_objective;

        if (m_is_minimization && objective < m_objective.value()) {
            score.is_objective_improvable = true;
        } else if (!m_is_minimization && objective < -m_objective.value()) {
            score.is_objective_improvable = true;
        } else {
            score.is_objective_improvable = false;
        }
        score.is_constraint_improvable = is_constraint_improvable;
        score.is_feasible              = is_feasible;

        return score;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr std::vector<ValueProxy<T_Value>>
    generate_variable_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> variable_parameter_proxies;

        for (const auto &variable_proxy : m_variable_proxies) {
            ValueProxy<T_Value> variable_parameter_proxy(
                variable_proxy.id(), variable_proxy.shape());
            variable_parameter_proxy.fill(a_VALUE);
            variable_parameter_proxies.push_back(variable_parameter_proxy);
        }
        return variable_parameter_proxies;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr std::vector<ValueProxy<T_Value>>
    generate_expression_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> expression_parameter_proxies;

        for (const auto &expression_proxy : m_expression_proxies) {
            ValueProxy<T_Value> expression_parameter_proxy(
                expression_proxy.id(), expression_proxy.shape());
            expression_parameter_proxy.fill(a_VALUE);
            expression_parameter_proxies.push_back(expression_parameter_proxy);
        }
        return expression_parameter_proxies;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr std::vector<ValueProxy<T_Value>>
    generate_constraint_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> constraint_parameter_proxies;

        for (const auto &constraint_proxy : m_constraint_proxies) {
            ValueProxy<T_Value> constraint_parameter_proxy(
                constraint_proxy.id(), constraint_proxy.shape());
            constraint_parameter_proxy.fill(a_VALUE);
            constraint_parameter_proxies.push_back(constraint_parameter_proxy);
        }
        return constraint_parameter_proxies;
    }

    /*************************************************************************/
    inline Solution<T_Variable, T_Expression> export_solution(void) const {
        // cannot be constexpr by clang
        Solution<T_Variable, T_Expression> solution;

        for (const auto &proxy : m_variable_proxies) {
            solution.variable_value_proxies.push_back(proxy.export_values());
        }

        for (const auto &proxy : m_expression_proxies) {
            solution.expression_value_proxies.push_back(proxy.export_values());
        }

        for (const auto &proxy : m_constraint_proxies) {
            solution.constraint_value_proxies.push_back(proxy.export_values());
            solution.violation_value_proxies.push_back(
                proxy.export_violations());
        }

        solution.objective = m_objective.value();
        bool is_feasible   = true;
        for (const auto &proxy : solution.violation_value_proxies) {
            if (utility::max(proxy.flat_indexed_values()) > 0) {
                is_feasible = false;
                break;
            }
        }
        solution.is_feasible = is_feasible;

        return solution;
    }

    /*************************************************************************/
    inline NamedSolution<T_Variable, T_Expression> convert_to_named_solution(
        const Solution<T_Variable, T_Expression> &a_SOLUTION) const {
        /// cannot be constexpr by clang
        NamedSolution<T_Variable, T_Expression> named_solution;

        for (std::size_t i = 0; i < m_variable_names.size(); i++) {
            named_solution.m_variable_value_proxies[m_variable_names[i]] =
                a_SOLUTION.variable_value_proxies[i];
        }

        for (std::size_t i = 0; i < m_expression_names.size(); i++) {
            named_solution.m_expression_value_proxies[m_expression_names[i]] =
                a_SOLUTION.expression_value_proxies[i];
        }

        for (std::size_t i = 0; i < m_constraint_names.size(); i++) {
            named_solution.m_constraint_value_proxies[m_constraint_names[i]] =
                a_SOLUTION.constraint_value_proxies[i];
        }

        for (std::size_t i = 0; i < m_constraint_names.size(); i++) {
            named_solution.m_violation_value_proxies[m_constraint_names[i]] =
                a_SOLUTION.violation_value_proxies[i];
        }

        named_solution.m_objective   = a_SOLUTION.objective;
        named_solution.m_is_feasible = a_SOLUTION.is_feasible;

        return named_solution;
    }

    /*************************************************************************/
    inline constexpr const std::vector<VariableProxy<T_Variable, T_Expression>>
        &variable_proxies(void) const {
        return m_variable_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        ExpressionProxy<T_Variable, T_Expression>>
        &expression_proxies(void) const {
        return m_expression_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        ConstraintProxy<T_Variable, T_Expression>>
        &constraint_proxies(void) const {
        return m_constraint_proxies;
    }

    /*************************************************************************/
    inline constexpr const Objective<T_Variable, T_Expression> &objective(
        void) const {
        return m_objective;
    }

    /*************************************************************************/
    inline constexpr const std::vector<std::string> &variable_names(
        void) const {
        return m_variable_names;
    }

    /*************************************************************************/
    inline constexpr const std::vector<std::string> &expression_names(
        void) const {
        return m_expression_names;
    }

    /*************************************************************************/
    inline constexpr const std::vector<std::string> &constraint_names(
        void) const {
        return m_constraint_names;
    }
};
using IPModel = Model<int, double>;
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/