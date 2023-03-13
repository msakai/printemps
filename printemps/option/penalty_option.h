/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_PENALTY_OPTION_H__
#define PRINTEMPS_OPTION_PENALTY_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct PenaltyOptionConstant {
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE    = 0.9;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_TIGHTENING_RATE  = 1.0;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_UPDATING_BALANCE = 0.0;
    static constexpr double DEFAULT_INITIAL_PENALTY_COEFFICIENT          = 1E7;
    static constexpr bool DEFAULT_IS_ENABLED_GROUP_PENALTY_COEFFICIENT = false;
};

/*****************************************************************************/
struct PenaltyOption {
    double penalty_coefficient_relaxing_rate;
    double penalty_coefficient_tightening_rate;
    double penalty_coefficient_updating_balance;
    double initial_penalty_coefficient;
    bool   is_enabled_group_penalty_coefficient;

    /*************************************************************************/
    PenaltyOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    PenaltyOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->penalty_coefficient_relaxing_rate =
            PenaltyOptionConstant::DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE;
        this->penalty_coefficient_tightening_rate =
            PenaltyOptionConstant::DEFAULT_PENALTY_COEFFICIENT_TIGHTENING_RATE;
        this->penalty_coefficient_updating_balance =
            PenaltyOptionConstant::DEFAULT_PENALTY_COEFFICIENT_UPDATING_BALANCE;
        this->initial_penalty_coefficient =
            PenaltyOptionConstant::DEFAULT_INITIAL_PENALTY_COEFFICIENT;
        this->is_enabled_group_penalty_coefficient =
            PenaltyOptionConstant::DEFAULT_IS_ENABLED_GROUP_PENALTY_COEFFICIENT;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                                          //
            " -- penalty.penalty_coefficient_relaxing_rate: " +  //
            utility::to_string(                                  //
                this->penalty_coefficient_relaxing_rate, "%f"));

        utility::print(                                            //
            " -- penalty.penalty_coefficient_tightening_rate: " +  //
            utility::to_string(                                    //
                this->penalty_coefficient_tightening_rate, "%f"));

        utility::print(                                             //
            " -- penalty.penalty_coefficient_updating_balance: " +  //
            utility::to_string(                                     //
                this->penalty_coefficient_updating_balance, "%f"));

        utility::print(                                    //
            " -- penalty.initial_penalty_coefficient: " +  //
            utility::to_string(                            //
                this->initial_penalty_coefficient, "%f"));

        utility::print(                                             //
            " -- penalty.is_enabled_group_penalty_coefficient: " +  //
            utility::to_true_or_false(                              //
                this->is_enabled_group_penalty_coefficient));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(                                     //
            &this->penalty_coefficient_relaxing_rate,  //
            "penalty_coefficient_relaxing_rate", a_OBJECT);

        read_json(                                       //
            &this->penalty_coefficient_tightening_rate,  //
            "penalty_coefficient_tightening_rate", a_OBJECT);

        read_json(                                        //
            &this->penalty_coefficient_updating_balance,  //
            "penalty_coefficient_updating_balance", a_OBJECT);

        read_json(                               //
            &this->initial_penalty_coefficient,  //
            "initial_penalty_coefficient", a_OBJECT);

        read_json(                                        //
            &this->is_enabled_group_penalty_coefficient,  //
            "is_enabled_group_penalty_coefficient", a_OBJECT);
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back(                         //
            "penalty_coefficient_relaxing_rate",  //
            this->penalty_coefficient_relaxing_rate);

        obj.emplace_back(                           //
            "penalty_coefficient_tightening_rate",  //
            this->penalty_coefficient_tightening_rate);

        obj.emplace_back(                            //
            "penalty_coefficient_updating_balance",  //
            this->penalty_coefficient_updating_balance);

        obj.emplace_back(                   //
            "initial_penalty_coefficient",  //
            this->initial_penalty_coefficient);

        obj.emplace_back(                            //
            "is_enabled_group_penalty_coefficient",  //
            this->is_enabled_group_penalty_coefficient);
        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/