/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestConstraintBinary : public ::testing::Test {
   protected:
    utility::UniformRandom<std::uniform_int_distribution<>, int>
        m_random_integer;
    utility::UniformRandom<std::uniform_int_distribution<>, int>
        m_random_positive_integer;

    virtual void SetUp(void) {
        m_random_integer.setup(-1000, 1000, 0);
        m_random_positive_integer.setup(1, 1000, 0);
    }
    virtual void TearDown() {
        /// nothing to do
    }
    int random_integer(void) {
        return m_random_integer.generate_random();
    }

    int random_positive_integer(void) {
        return m_random_positive_integer.generate_random();
    }
};

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_lower) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable <= Variable
    {
        auto constraint = variable <= variable;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Variable <= Integer
    {
        auto constraint = variable <= constant;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    ///  Integer <= Variable
    {
        auto constraint = constant <= variable;

        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Variable <= VariableProxy
    {
        auto constraint = variable <= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// VariableProxy <= Variable
    {
        auto constraint = variable_proxy <= variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Variable <= Expression
    {
        auto constraint = variable <= expression;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Expression <= Variable
    {
        auto constraint = expression <= variable;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Variable <= ExpressionProxy
    {
        auto constraint = variable <= expression_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// ExpressionProxy <= Variable
    {
        auto constraint = expression_proxy <= variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_equal) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable <= Variable
    {
        auto constraint = variable == variable;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Variable == Integer
    {
        auto constraint = variable == constant;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    ///  Integer == Variable
    {
        auto constraint = constant == variable;

        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Variable == VariableProxy
    {
        auto constraint = variable == variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// VariableProxy == Variable
    {
        auto constraint = variable_proxy == variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Variable == Expression
    {
        auto constraint = variable == expression;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Expression == Variable
    {
        auto constraint = expression == variable;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Variable == ExpressionProxy
    {
        auto constraint = variable == expression_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// ExpressionProxy == Variable
    {
        auto constraint = expression_proxy == variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_upper) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable >= Variable
    {
        auto constraint = variable >= variable;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Variable >= Integer
    {
        auto constraint = variable >= constant;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    ///  Integer >= Variable
    {
        auto constraint = constant >= variable;

        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Variable >= VariableProxy
    {
        auto constraint = variable >= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// VariableProxy >= Variable
    {
        auto constraint = variable_proxy >= variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Variable >= Expression
    {
        auto constraint = variable >= expression;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Expression >= Variable
    {
        auto constraint = expression >= variable;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Variable >= ExpressionProxy
    {
        auto constraint = variable >= expression_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// ExpressionProxy >= Variable
    {
        auto constraint = expression_proxy >= variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_proxy_lower) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy <= VariableProxy
    {
        auto constraint = variable_proxy <= variable_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// VariableProxy <= Integer
    {
        auto constraint = variable_proxy <= constant;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    ///  Integer <= VariableProxy
    {
        auto constraint = constant <= variable_proxy;

        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// VariableProxy <= Variable
    {
        auto constraint = variable_proxy <= variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Variable <= VariableProxy
    {
        auto constraint = variable <= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// VariableProxy <= Expression
    {
        auto constraint = variable_proxy <= expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Expression <= VariableProxy
    {
        auto constraint = expression <= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// VariableProxy <= ExpressionProxy
    {
        auto constraint = variable_proxy <= expression_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// ExpressionProxy <= VariableProxy
    {
        auto constraint = expression_proxy <= variable_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_proxy_equal) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy == VariableProxy
    {
        auto constraint = variable_proxy == variable_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// VariableProxy == Integer
    {
        auto constraint = variable_proxy == constant;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    ///  Integer == VariableProxy
    {
        auto constraint = constant == variable_proxy;

        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// VariableProxy <= Variable
    {
        auto constraint = variable_proxy == variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Variable <= VariableProxy
    {
        auto constraint = variable == variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// VariableProxy == Expression
    {
        auto constraint = variable_proxy == expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Expression == VariableProxy
    {
        auto constraint = expression == variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// VariableProxy == ExpressionProxy
    {
        auto constraint = variable_proxy == expression_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// ExpressionProxy == VariableProxy
    {
        auto constraint = expression_proxy == variable_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_proxy_upper) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy >= VariableProxy
    {
        auto constraint = variable_proxy >= variable_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// VariableProxy >= Integer
    {
        auto constraint = variable_proxy >= constant;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    ///  Integer >= VariableProxy
    {
        auto constraint = constant >= variable_proxy;

        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// VariableProxy <= Variable
    {
        auto constraint = variable_proxy >= variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Variable <= VariableProxy
    {
        auto constraint = variable >= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// VariableProxy >= Expression
    {
        auto constraint = variable_proxy >= expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Expression >= VariableProxy
    {
        auto constraint = expression >= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// VariableProxy >= ExpressionProxy
    {
        auto constraint = variable_proxy >= expression_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// ExpressionProxy >= VariableProxy
    {
        auto constraint = expression_proxy >= variable_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_lower) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression <= Expression
    {
        auto constraint = expression <= expression;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Expression <= Integer
    {
        auto constraint = expression <= constant;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    ///  Integer <= Expression
    {
        auto constraint = constant <= expression;

        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Expression <= Variable
    {
        auto constraint = expression <= variable;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Variable <= Expression
    {
        auto constraint = variable <= expression;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Expression <= VariableProxy
    {
        auto constraint = expression <= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// VariableProxy <= Expression
    {
        auto constraint = variable_proxy <= expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Expression <= ExpressionProxy
    {
        auto constraint = expression <= expression_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// ExpressionProxy <= Expression
    {
        auto constraint = expression_proxy <= expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_equal) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression == Expression
    {
        auto constraint = expression == expression;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Expression == Integer
    {
        auto constraint = expression == constant;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    ///  Integer == Expression
    {
        auto constraint = constant == expression;

        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Expression == Variable
    {
        auto constraint = expression == variable;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Variable == Expression
    {
        auto constraint = variable == expression;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Expression == VariableProxy
    {
        auto constraint = expression == variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// VariableProxy == Expression
    {
        auto constraint = variable_proxy == expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Expression == ExpressionProxy
    {
        auto constraint = expression == expression_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// ExpressionProxy == Expression
    {
        auto constraint = expression_proxy == expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_upper) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression >= Expression
    {
        auto constraint = expression >= expression;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Expression >= Integer
    {
        auto constraint = expression >= constant;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    ///  Integer >= Expression
    {
        auto constraint = constant >= expression;

        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Expression >= Variable
    {
        auto constraint = expression >= variable;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Variable >= Expression
    {
        auto constraint = variable >= expression;

        EXPECT_EQ(0, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Expression >= VariableProxy
    {
        auto constraint = expression >= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// VariableProxy >= Expression
    {
        auto constraint = variable_proxy >= expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Expression >= ExpressionProxy
    {
        auto constraint = expression >= expression_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// ExpressionProxy >= Expression
    {
        auto constraint = expression_proxy >= expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_proxy_lower) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy <= ExpressionProxy
    {
        auto constraint = expression_proxy <= expression_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// ExpressionProxy <= Integer
    {
        auto constraint = expression_proxy <= constant;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    ///  Integer <= ExpressionProxy
    {
        auto constraint = constant <= expression_proxy;

        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// ExpressionProxy <= Variable
    {
        auto constraint = expression_proxy <= variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Variable <= ExpressionProxy
    {
        auto constraint = variable <= expression_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// ExpressionProxy <= Expression
    {
        auto constraint = variable_proxy <= expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }

    /// Expression <= ExpressionProxy
    {
        auto constraint = expression <= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_proxy_equal) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy == ExpressionProxy
    {
        auto constraint = expression_proxy == expression_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// ExpressionProxy == Integer
    {
        auto constraint = expression_proxy == constant;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    ///  Integer == ExpressionProxy
    {
        auto constraint = constant == expression_proxy;

        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// ExpressionProxy == Variable
    {
        auto constraint = expression_proxy == variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Variable == ExpressionProxy
    {
        auto constraint = variable == expression_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// ExpressionProxy == Expression
    {
        auto constraint = variable_proxy == expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }

    /// Expression == ExpressionProxy
    {
        auto constraint = expression == variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_proxy_upper) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy >= ExpressionProxy
    {
        auto constraint = expression_proxy >= expression_proxy;

        EXPECT_EQ(
            0, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// ExpressionProxy >= Integer
    {
        auto constraint = expression_proxy >= constant;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    ///  Integer >= ExpressionProxy
    {
        auto constraint = constant >= expression_proxy;

        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// ExpressionProxy >= Variable
    {
        auto constraint = expression_proxy >= variable;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Variable >= ExpressionProxy
    {
        auto constraint = variable >= expression_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// ExpressionProxy >= Expression
    {
        auto constraint = variable_proxy >= expression;

        EXPECT_EQ(
            1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }

    /// Expression >= ExpressionProxy
    {
        auto constraint = expression >= variable_proxy;

        EXPECT_EQ(1, constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(
            -1, constraint.expression().sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/