/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_EXPRESSION_H__
#define PRINTEMPS_MODEL_COMPONENT_EXPRESSION_H__

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move;
}  // namespace neighborhood
}  // namespace printemps

namespace printemps {
namespace model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Objective;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
struct ExpressionConstant {
    static constexpr int DEFAULT_SENSITIVITY_RESERVE_SIZE = 1000;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression : public multi_array::AbstractMultiArrayElement {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : default, private (for std::vector.reserve())
     *  -- Copy assignment     : default  public
     *  -- Move constructor    : default, public
     *  -- Move assignment     : default, public
     *
     * NOTE: The Move constructor is enabled and active, so that declaring an
     * Expression object by Expression<int,int> e1 = e2.copy(); is permissible.
     * This is due to describing definitions of binary operators.
     */

    /**
     * These friend declarations are required to have an Expression object as a
     * member.
     */
    friend class Objective<T_Variable, T_Expression>;
    friend class Constraint<T_Variable, T_Expression>;

   private:
    T_Expression m_constant_value;
    T_Expression m_value;
    bool         m_is_enabled;

    std::unordered_map<Variable<T_Variable, T_Expression> *, T_Expression>
        m_sensitivities;

    utility::FixedSizeHashMap<Variable<T_Variable, T_Expression> *,
                              T_Expression>
        m_fixed_sensitivities;

    std::uint64_t m_plus_one_coefficient_mask;
    std::uint64_t m_minus_one_coefficient_mask;
    bool          m_has_effective_plus_one_coefficient_mask;
    bool          m_has_effective_minus_one_coefficient_mask;

    /*************************************************************************/
    /// Default constructor
    Expression(void) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Expression(const Expression<T_Variable, T_Expression> &) = default;

    /*************************************************************************/
    Expression(const std::unordered_map<Variable<T_Variable, T_Expression> *,
                                        T_Expression> &a_SENSITIVITIES,
               const T_Expression                      a_CONSTANT_VALUE) {
        this->initialize();
        m_sensitivities  = a_SENSITIVITIES;
        m_constant_value = a_CONSTANT_VALUE;
    }

   public:
    /*************************************************************************/
    /// Copy assignment
    /**
     * NOTE: The members proxy_index and flat_index will be discarded in the
     * copy assignment and related methods. It's not a problem because the
     * program does not use flat_index at the moment.
     */
    Expression<T_Variable, T_Expression> &operator    =(
        const Expression<T_Variable, T_Expression> &) = default;

    /*************************************************************************/
    /// Move constructor
    Expression(Expression<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    Expression<T_Variable, T_Expression> &operator=(
        Expression<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    inline static constexpr Expression<T_Variable, T_Expression>
    create_instance(void) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Expression<T_Variable, T_Expression> expression;
        return expression;
    }

    /*************************************************************************/
    inline static constexpr Expression<T_Variable, T_Expression>
    create_instance(
        const std::unordered_map<Variable<T_Variable, T_Expression> *,
                                 T_Expression> &a_sensitivity,
        const T_Expression                      a_CONSTANT_VALUE) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Expression<T_Variable, T_Expression> expression(a_sensitivity,
                                                        a_CONSTANT_VALUE);
        return expression;
    }

    /*************************************************************************/
    void initialize(void) {
        multi_array::AbstractMultiArrayElement::initialize();
        m_constant_value = 0;
        m_value          = 0;
        m_is_enabled     = true;
        m_sensitivities.clear();
        m_fixed_sensitivities.initialize();

        m_plus_one_coefficient_mask                = 0;
        m_minus_one_coefficient_mask               = 0;
        m_has_effective_plus_one_coefficient_mask  = false;
        m_has_effective_minus_one_coefficient_mask = false;
    }

    /*************************************************************************/
    inline constexpr void set_sensitivities(
        const std::unordered_map<Variable<T_Variable, T_Expression> *,
                                 T_Expression> &a_SENSITIVITIES) {
        m_sensitivities = a_SENSITIVITIES;
    }

    /*************************************************************************/
    inline constexpr std::unordered_map<Variable<T_Variable, T_Expression> *,
                                        T_Expression>
        &sensitivities(void) {
        return m_sensitivities;
    }

    /*************************************************************************/
    inline constexpr const std::unordered_map<
        Variable<T_Variable, T_Expression> *, T_Expression>
        &sensitivities(void) const {
        return m_sensitivities;
    }

    /*************************************************************************/
    inline constexpr void setup_fixed_sensitivities(void) {
        /**
         * std::unordered_map is slow in hashing because it uses modulo
         * operations. For efficient evaluations of solutions, a hash map
         * without modulo operations is set up by converting from the
         * std::unordered map.
         */
        m_fixed_sensitivities.setup(m_sensitivities,
                                    sizeof(Variable<T_Variable, T_Expression>));
    }

    /*************************************************************************/
    inline constexpr void setup_mask(void) {
        std::uint64_t plus_one_coefficient_mask  = 0;
        std::uint64_t minus_one_coefficient_mask = 0;

        for (const auto &sensitivity : m_sensitivities) {
            if (!(fabs(sensitivity.second - 1.0) < constant::EPSILON_10)) {
                plus_one_coefficient_mask =
                    plus_one_coefficient_mask |
                    reinterpret_cast<std::uint64_t>(sensitivity.first);
            }
            if (!(fabs(sensitivity.second + 1.0) < constant::EPSILON_10)) {
                minus_one_coefficient_mask =
                    minus_one_coefficient_mask |
                    reinterpret_cast<std::uint64_t>(sensitivity.first);
            }
        }

        m_plus_one_coefficient_mask  = ~plus_one_coefficient_mask;
        m_minus_one_coefficient_mask = ~minus_one_coefficient_mask;

        int number_of_fast_computable_plus_one_coefficients  = 0;
        int number_of_fast_computable_minus_one_coefficients = 0;
        for (const auto &sensitivity : m_sensitivities) {
            if (fabs(sensitivity.second - 1.0) < constant::EPSILON_10) {
                if (reinterpret_cast<std::uint64_t>(sensitivity.first) &
                    m_plus_one_coefficient_mask) {
                    number_of_fast_computable_plus_one_coefficients++;
                }
            }
            if (fabs(sensitivity.second + 1.0) < constant::EPSILON_10) {
                if (reinterpret_cast<std::uint64_t>(sensitivity.first) &
                    m_minus_one_coefficient_mask) {
                    number_of_fast_computable_minus_one_coefficients++;
                }
            }
        }

        int number_of_variables = m_sensitivities.size();
        if (2 * number_of_fast_computable_plus_one_coefficients >=
            number_of_variables) {
            m_has_effective_plus_one_coefficient_mask = true;
        } else {
            m_has_effective_plus_one_coefficient_mask = false;
        }

        if (2 * number_of_fast_computable_minus_one_coefficients >=
            number_of_variables) {
            m_has_effective_minus_one_coefficient_mask = true;
        } else {
            m_has_effective_minus_one_coefficient_mask = false;
        }
    }

    /*************************************************************************/
    inline constexpr T_Expression constant_value(void) const {
        return m_constant_value;
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(void) const noexcept {
        T_Expression value = m_constant_value;

        for (const auto &sensitivity : m_sensitivities) {
            value += sensitivity.first->value() * sensitivity.second;
        }
        return value;
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE)
        const noexcept {
        /// The following code is required for nonlinear objective functions.
#ifndef _MPS_SOLVER
        if (a_MOVE.alterations.size() == 0) {
            return this->evaluate();
        }
#endif

        T_Expression new_value = m_value;
        for (const auto &alteration : a_MOVE.alterations) {
            new_value += m_fixed_sensitivities.at(alteration.first) *
                         (alteration.second - alteration.first->value());
        }
        return new_value;
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_with_mask(
        model_component::Variable<T_Variable, T_Expression> *a_variable_ptr,
        const T_Variable a_TARGET_VALUE) const noexcept {
        if (m_has_effective_plus_one_coefficient_mask &&
            reinterpret_cast<std::uint64_t>(a_variable_ptr) &
                m_plus_one_coefficient_mask) {
            return m_value + (a_TARGET_VALUE - a_variable_ptr->value());
        }
        if (m_has_effective_minus_one_coefficient_mask &&
            reinterpret_cast<std::uint64_t>(a_variable_ptr) &
                m_minus_one_coefficient_mask) {
            return m_value - (a_TARGET_VALUE - a_variable_ptr->value());
        }

        return m_value + m_fixed_sensitivities.at(a_variable_ptr) *
                             (a_TARGET_VALUE - a_variable_ptr->value());
    }

    /*************************************************************************/
    inline constexpr void update(void) {
        m_value = this->evaluate();
    }

    /*************************************************************************/
    inline constexpr void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
        m_value = this->evaluate(a_MOVE);
    }

    /*************************************************************************/
    inline constexpr T_Expression value(void) const noexcept {
        return m_value;
    }

    /*************************************************************************/
    inline constexpr const Expression<T_Variable, T_Expression> &self(
        void) const {
        return *this;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> copy(void) const {
        return create_instance(this->sensitivities(), this->constant_value());
    }

    /*************************************************************************/
    inline constexpr bool is_enabled(void) const {
        return m_is_enabled;
    }

    /*************************************************************************/
    inline constexpr void enable(void) {
        m_is_enabled = true;
    }

    /*************************************************************************/
    inline constexpr void disable(void) {
        m_is_enabled = false;
    }

    /*************************************************************************/
    inline constexpr void erase(
        Variable<T_Variable, T_Expression> *a_variable_ptr) {
        m_sensitivities.erase(a_variable_ptr);
    }

    /*************************************************************************/
    inline constexpr void substitute(
        Variable<T_Variable, T_Expression> *        a_variable_ptr,
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        *this += m_sensitivities[a_variable_ptr] * a_EXPRESSION;
        m_sensitivities.erase(a_variable_ptr);
    }

    /*************************************************************************/
    inline constexpr T_Expression lower_bound(void) {
        T_Expression lower_bound = m_constant_value;
        for (const auto &sensitivity : m_sensitivities) {
            if (sensitivity.first->is_fixed()) {
                lower_bound += sensitivity.second * sensitivity.first->value();
            } else {
                auto value = (sensitivity.second > 0)
                                 ? sensitivity.first->lower_bound()
                                 : sensitivity.first->upper_bound();

                lower_bound += sensitivity.second * value;
            }
        }
        return lower_bound;
    }

    /*************************************************************************/
    inline constexpr T_Expression upper_bound(void) {
        T_Expression upper_bound = m_constant_value;
        for (const auto &sensitivity : m_sensitivities) {
            if (sensitivity.first->is_fixed()) {
                upper_bound += sensitivity.second * sensitivity.first->value();
            } else {
                auto value = (sensitivity.second > 0)
                                 ? sensitivity.first->upper_bound()
                                 : sensitivity.first->lower_bound();

                upper_bound += sensitivity.second * value;
            }
        }
        return upper_bound;
    }

    /*************************************************************************/
    inline constexpr T_Expression fixed_term_value(void) {
        int fixed_term_value = 0;
        for (const auto &sensitivity : m_sensitivities) {
            if (sensitivity.first->is_fixed()) {
                fixed_term_value +=
                    sensitivity.second * sensitivity.first->value();
            }
        }
        return fixed_term_value;
    }

    /*************************************************************************/
    inline std::unordered_map<
        model_component::Variable<T_Variable, T_Expression> *, T_Expression>
    mutable_variable_sensitivities(void) const {
        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *, T_Expression>
            mutable_variable_sensitivities;

        for (const auto &sensitivity : m_sensitivities) {
            if (!sensitivity.first->is_fixed()) {
                mutable_variable_sensitivities[sensitivity.first] =
                    sensitivity.second;
            }
        }
        return mutable_variable_sensitivities;
    }

    /*************************************************************************/
    inline std::unordered_map<
        model_component::Variable<T_Variable, T_Expression> *, T_Expression>
    positive_mutable_variable_sensitivities(void) const {
        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *, T_Expression>
            positive_mutable_variable_sensitivities;

        for (const auto &sensitivity : m_sensitivities) {
            if (!sensitivity.first->is_fixed() && sensitivity.second > 0) {
                positive_mutable_variable_sensitivities[sensitivity.first] =
                    sensitivity.second;
            }
        }
        return positive_mutable_variable_sensitivities;
    }

    /*************************************************************************/
    inline std::unordered_map<
        model_component::Variable<T_Variable, T_Expression> *, T_Expression>
    negative_mutable_variable_sensitivities(void) const {
        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *, T_Expression>
            negative_mutable_variable_sensitivities;

        for (const auto &sensitivity : m_sensitivities) {
            if (!sensitivity.first->is_fixed() && sensitivity.second < 0) {
                negative_mutable_variable_sensitivities[sensitivity.first] =
                    sensitivity.second;
            }
        }
        return negative_mutable_variable_sensitivities;
    }

    /*************************************************************************/
    inline constexpr std::uint64_t plus_one_coefficient_mask(
        void) const noexcept {
        return m_plus_one_coefficient_mask;
    }

    /*************************************************************************/
    inline constexpr std::uint64_t minus_one_coefficient_mask(
        void) const noexcept {
        return m_minus_one_coefficient_mask;
    }

    /*************************************************************************/
    inline constexpr bool has_effective_plus_one_coefficient_mask(
        void) const noexcept {
        return m_has_effective_plus_one_coefficient_mask;
    }

    /*************************************************************************/
    inline constexpr bool has_effective_minus_one_coefficient_mask(
        void) const noexcept {
        return m_has_effective_minus_one_coefficient_mask;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> operator+(
        void) const {
        return create_instance(this->sensitivities(), this->constant_value());
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> operator-(
        void) const {
        auto result =
            create_instance(this->sensitivities(), this->constant_value());
        for (auto &&sensitivity : result.m_sensitivities) {
            sensitivity.second *= -1;
        }
        result.m_constant_value *= -1;
        result.m_value *= -1;
        return result;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator=(
        const T_Value a_VALUE) {
        m_sensitivities.clear();
        m_constant_value = a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr Expression<T_Variable, T_Expression> &operator=(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        m_sensitivities  = a_EXPRESSION_LIKE.to_expression().sensitivities();
        m_constant_value = 0;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr Expression<T_Variable, T_Expression> &operator=(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        m_sensitivities  = a_EXPRESSION.sensitivities();
        m_constant_value = a_EXPRESSION.constant_value();
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator+=(
        const T_Value a_VALUE) {
        m_constant_value += a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr Expression<T_Variable, T_Expression> &operator+=(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        *this += a_EXPRESSION_LIKE.to_expression();
        return *this;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> &operator+=(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        for (const auto &append : a_EXPRESSION.m_sensitivities) {
            if (m_sensitivities.find(append.first) != m_sensitivities.end()) {
                m_sensitivities[append.first] += append.second;
            } else {
                m_sensitivities[append.first] = append.second;
            }
        }

        m_constant_value += a_EXPRESSION.m_constant_value;
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator-=(
        const T_Value a_VALUE) {
        m_constant_value -= a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr Expression<T_Variable, T_Expression> &operator-=(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        *this -= a_EXPRESSION_LIKE.to_expression();
        return *this;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> &operator-=(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        *this += (-a_EXPRESSION);
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator*=(
        const T_Value a_VALUE) {
        for (auto &&sensitivity : m_sensitivities) {
            sensitivity.second *= a_VALUE;
        }
        m_constant_value *= a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator/=(
        const T_Value a_VALUE) {
        for (auto &&sensitivity : m_sensitivities) {
            sensitivity.second /= a_VALUE;
        }
        m_constant_value /= a_VALUE;
        return *this;
    }
};
using IPExpression = Expression<int, double>;
}  // namespace model_component
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/