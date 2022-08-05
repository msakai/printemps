/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_OPTION_H__
#define PRINTEMPS_OPTION_OPTION_H__

#include "enumerate/enumerate.h"

#include "lagrange_dual_option.h"
#include "local_search_option.h"
#include "tabu_search_option.h"

namespace printemps::option {
/*****************************************************************************/
struct OptionConstant {
    static constexpr int    DEFAULT_ITERATION_MAX                       = 100;
    static constexpr double DEFAULT_TIME_MAX                            = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET                         = 0.0;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE   = 0.9;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_TIGHTENING_RATE = 1.0;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_UPDATING_BALANCE = 0.0;
    static constexpr double DEFAULT_INITIAL_PENALTY_COEFFICIENT          = 1E7;
    static constexpr bool   DEFAULT_IS_ENABLED_LAGRANGE_DUAL = false;
    static constexpr bool   DEFAULT_IS_ENABLED_LOCAL_SEARCH  = true;
    static constexpr bool   DEFAULT_IS_ENABLED_GROUPING_PENALTY_COEFFICIENT =
        false;
    static constexpr bool DEFAULT_IS_ENABLED_PRESOLVE                 = true;
    static constexpr bool DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION = true;
    static constexpr bool DEFAULT_IS_ENABLED_PARALLEL_EVALUATION      = true;
    static constexpr bool DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE =
        true;
    static constexpr bool DEFAULT_IS_ENABLED_BINARY_MOVE         = true;
    static constexpr bool DEFAULT_IS_ENABLED_INTEGER_MOVE        = true;
    static constexpr bool DEFAULT_IS_ENABLED_AGGREGATION_MOVE    = true;
    static constexpr bool DEFAULT_IS_ENABLED_PRECEDENCE_MOVE     = false;
    static constexpr bool DEFAULT_IS_ENABLED_VARIABLE_BOUND_MOVE = false;
    static constexpr bool DEFAULT_IS_ENABLED_SOFT_SELECTION_MOVE = false;
    static constexpr bool DEFAULT_IS_ENABLED_CHAIN_MOVE          = true;
    static constexpr bool DEFAULT_IS_ENABLED_TWO_FLIP_MOVE       = false;
    static constexpr bool DEFAULT_IS_ENABLED_USER_DEFINED_MOVE   = false;
    static constexpr int  DEFAULT_CHAIN_MOVE_CAPACITY            = 10000;
    static constexpr chain_move_reduce_mode::ChainMoveReduceMode
                            DEFAULT_CHAIN_MOVE_REDUCE_MODE = chain_move_reduce_mode::OverlapRate;
    static constexpr double DEFAULT_CHAIN_MOVE_OVERLAP_RATE_THRESHOLD = 0.2;

    static constexpr selection_mode::SelectionMode DEFAULT_SELECTION_MODE =
        selection_mode::Independent;
    static constexpr improvability_screening_mode::ImprovabilityScreeningMode
        DEFAULT_IMPROVABILITY_SCREENING_MODE =
            improvability_screening_mode::Automatic;

    static constexpr double DEFAULT_TARGET_OBJECTIVE       = -1E100;
    static constexpr bool   DEFAULT_SEED                   = 1;
    static constexpr bool   DEFAULT_VERBOSE                = verbose::None;
    static constexpr bool   DEFAULT_IS_ENABLED_WRITE_TREND = false;
    static constexpr bool   DEFAULT_IS_ENABLED_STORE_FEASIBLE_SOLUTIONS = false;
    static constexpr int    DEFAULT_FEASIBLE_SOLUTIONS_CAPACITY         = 1000;
};

/*****************************************************************************/
struct Option {
    int    iteration_max;
    double time_offset;  // hidden
    double time_max;
    double penalty_coefficient_relaxing_rate;
    double penalty_coefficient_tightening_rate;
    double penalty_coefficient_updating_balance;  // hidden
    double initial_penalty_coefficient;
    bool   is_enabled_lagrange_dual;
    bool   is_enabled_local_search;
    bool   is_enabled_grouping_penalty_coefficient;
    bool   is_enabled_presolve;
    bool   is_enabled_initial_value_correction;
    bool   is_enabled_parallel_evaluation;
    bool   is_enabled_parallel_neighborhood_update;

    bool is_enabled_binary_move;
    bool is_enabled_integer_move;
    bool is_enabled_aggregation_move;
    bool is_enabled_precedence_move;
    bool is_enabled_variable_bound_move;
    bool is_enabled_soft_selection_move;  // hidden
    bool is_enabled_chain_move;
    bool is_enabled_two_flip_move;  // hidden
    bool is_enabled_user_defined_move;

    int chain_move_capacity;  // hidden
    chain_move_reduce_mode::ChainMoveReduceMode
           chain_move_reduce_mode;             // hidden
    double chain_move_overlap_rate_threshold;  // hidden

    selection_mode::SelectionMode selection_mode;
    improvability_screening_mode::ImprovabilityScreeningMode
        improvability_screening_mode;

    double target_objective_value;
    int    seed;  // hidden
    int    verbose;
    bool   is_enabled_write_trend;               // hidden
    bool   is_enabled_store_feasible_solutions;  // hidden
    int    feasible_solutions_capacity;          // hidden

    TabuSearchOption   tabu_search;
    LocalSearchOption  local_search;
    LagrangeDualOption lagrange_dual;

    /*************************************************************************/
    Option(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->iteration_max = OptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = OptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = OptionConstant::DEFAULT_TIME_OFFSET;
        this->penalty_coefficient_relaxing_rate =
            OptionConstant::DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE;
        this->penalty_coefficient_tightening_rate =
            OptionConstant::DEFAULT_PENALTY_COEFFICIENT_TIGHTENING_RATE;
        this->penalty_coefficient_updating_balance =
            OptionConstant::DEFAULT_PENALTY_COEFFICIENT_UPDATING_BALANCE;
        this->initial_penalty_coefficient =
            OptionConstant::DEFAULT_INITIAL_PENALTY_COEFFICIENT;
        this->is_enabled_lagrange_dual =
            OptionConstant::DEFAULT_IS_ENABLED_LAGRANGE_DUAL;
        this->is_enabled_local_search =
            OptionConstant::DEFAULT_IS_ENABLED_LOCAL_SEARCH;
        this->is_enabled_grouping_penalty_coefficient =
            OptionConstant::DEFAULT_IS_ENABLED_GROUPING_PENALTY_COEFFICIENT;
        this->is_enabled_presolve = OptionConstant::DEFAULT_IS_ENABLED_PRESOLVE;
        this->is_enabled_initial_value_correction =
            OptionConstant::DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION;
        this->is_enabled_parallel_evaluation =
            OptionConstant::DEFAULT_IS_ENABLED_PARALLEL_EVALUATION;
        this->is_enabled_parallel_neighborhood_update =
            OptionConstant::DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE;
        this->is_enabled_binary_move =
            OptionConstant::DEFAULT_IS_ENABLED_BINARY_MOVE;
        this->is_enabled_integer_move =
            OptionConstant::DEFAULT_IS_ENABLED_INTEGER_MOVE;
        this->is_enabled_aggregation_move =
            OptionConstant::DEFAULT_IS_ENABLED_AGGREGATION_MOVE;
        this->is_enabled_precedence_move =
            OptionConstant::DEFAULT_IS_ENABLED_PRECEDENCE_MOVE;
        this->is_enabled_variable_bound_move =
            OptionConstant::DEFAULT_IS_ENABLED_VARIABLE_BOUND_MOVE;
        this->is_enabled_soft_selection_move =
            OptionConstant::DEFAULT_IS_ENABLED_SOFT_SELECTION_MOVE;
        this->is_enabled_chain_move =
            OptionConstant::DEFAULT_IS_ENABLED_CHAIN_MOVE;
        this->is_enabled_two_flip_move =
            OptionConstant::DEFAULT_IS_ENABLED_TWO_FLIP_MOVE;
        this->is_enabled_user_defined_move =
            OptionConstant::DEFAULT_IS_ENABLED_USER_DEFINED_MOVE;
        this->chain_move_capacity = OptionConstant::DEFAULT_CHAIN_MOVE_CAPACITY;
        this->chain_move_reduce_mode =
            OptionConstant::DEFAULT_CHAIN_MOVE_REDUCE_MODE;
        this->chain_move_overlap_rate_threshold =
            OptionConstant::DEFAULT_CHAIN_MOVE_OVERLAP_RATE_THRESHOLD;

        this->selection_mode = OptionConstant::DEFAULT_SELECTION_MODE;
        this->improvability_screening_mode =
            OptionConstant::DEFAULT_IMPROVABILITY_SCREENING_MODE;

        this->target_objective_value = OptionConstant::DEFAULT_TARGET_OBJECTIVE;
        this->seed                   = OptionConstant::DEFAULT_SEED;
        this->verbose                = OptionConstant::DEFAULT_VERBOSE;
        this->is_enabled_write_trend =
            OptionConstant::DEFAULT_IS_ENABLED_WRITE_TREND;
        this->is_enabled_store_feasible_solutions =
            OptionConstant::DEFAULT_IS_ENABLED_STORE_FEASIBLE_SOLUTIONS;
        this->feasible_solutions_capacity =
            OptionConstant::DEFAULT_FEASIBLE_SOLUTIONS_CAPACITY;

        this->lagrange_dual.initialize();
        this->local_search.initialize();
        this->tabu_search.initialize();
    }

    /*************************************************************************/
    void print(void) const {
        utility::print_single_line(true);
        utility::print_info("The values for options are specified as follows:",
                            true);

        utility::print(              //
            " -- iteration_max: " +  //
            utility::to_string(this->iteration_max, "%d"));

        utility::print(            //
            " -- time_offset: " +  //
            utility::to_string(this->time_offset, "%f"));

        utility::print(         //
            " -- time_max: " +  //
            utility::to_string(this->time_max, "%f"));

        utility::print(                                  //
            " -- penalty_coefficient_relaxing_rate: " +  //
            utility::to_string(this->penalty_coefficient_relaxing_rate, "%f"));

        utility::print(                                    //
            " -- penalty_coefficient_tightening_rate: " +  //
            utility::to_string(this->penalty_coefficient_tightening_rate,
                               "%f"));

        utility::print(                                     //
            " -- penalty_coefficient_updating_balance: " +  //
            utility::to_string(this->penalty_coefficient_updating_balance,
                               "%f"));

        utility::print(                            //
            " -- initial_penalty_coefficient: " +  //
            utility::to_string(this->initial_penalty_coefficient, "%f"));

        utility::print(                    //
            " -- is_enabled_presolve: " +  //
            utility::to_string(this->is_enabled_presolve, "%d"));

        utility::print(                         //
            " -- is_enabled_lagrange_dual: " +  //
            utility::to_string(this->is_enabled_lagrange_dual, "%d"));

        utility::print(                        //
            " -- is_enabled_local_search: " +  //
            utility::to_string(this->is_enabled_local_search, "%d"));

        utility::print(                                        //
            " -- is_enabled_grouping_penalty_coefficient: " +  //
            utility::to_string(this->is_enabled_grouping_penalty_coefficient,
                               "%d"));

        utility::print(                                    //
            " -- is_enabled_initial_value_correction: " +  //
            utility::to_string(this->is_enabled_initial_value_correction,
                               "%d"));

        utility::print(                               //
            " -- is_enabled_parallel_evaluation: " +  //
            utility::to_string(this->is_enabled_parallel_evaluation, "%d"));

        utility::print(                                        //
            " -- is_enabled_parallel_neighborhood_update: " +  //
            utility::to_string(this->is_enabled_parallel_neighborhood_update,
                               "%d"));

        utility::print(                       //
            " -- is_enabled_binary_move: " +  //
            utility::to_string(this->is_enabled_binary_move, "%d"));

        utility::print(                        //
            " -- is_enabled_integer_move: " +  //
            utility::to_string(this->is_enabled_integer_move, "%d"));

        utility::print(                            //
            " -- is_enabled_aggregation_move: " +  //
            utility::to_string(this->is_enabled_aggregation_move, "%d"));

        utility::print(                           //
            " -- is_enabled_precedence_move: " +  //
            utility::to_string(this->is_enabled_precedence_move, "%d"));

        utility::print(                               //
            " -- is_enabled_variable_bound_move: " +  //
            utility::to_string(this->is_enabled_variable_bound_move, "%d"));

        utility::print(                               //
            " -- is_enabled_soft_selection_move: " +  //
            utility::to_string(this->is_enabled_soft_selection_move, "%d"));

        utility::print(                      //
            " -- is_enabled_chain_move: " +  //
            utility::to_string(this->is_enabled_chain_move, "%d"));

        utility::print(                         //
            " -- is_enabled_two_flip_move: " +  //
            utility::to_string(this->is_enabled_two_flip_move, "%d"));

        utility::print(                             //
            " -- is_enabled_user_defined_move: " +  //
            utility::to_string(this->is_enabled_user_defined_move, "%d"));

        utility::print(                    //
            " -- chain_move_capacity: " +  //
            utility::to_string(this->chain_move_capacity, "%d"));

        utility::print(                       //
            " -- chain_move_reduce_mode: " +  //
            utility::to_string(this->chain_move_reduce_mode, "%d"));

        utility::print(                                  //
            " -- chain_move_overlap_rate_threshold: " +  //
            utility::to_string(this->chain_move_overlap_rate_threshold, "%f"));

        utility::print(               //
            " -- selection_mode: " +  //
            utility::to_string(this->selection_mode, "%d"));

        utility::print(                             //
            " -- improvability_screening_mode: " +  //
            utility::to_string(this->improvability_screening_mode, "%d"));

        utility::print(                       //
            " -- target_objective_value: " +  //
            utility::to_string(this->target_objective_value, "%.3e"));

        utility::print(     //
            " -- seed: " +  //
            utility::to_string(this->seed, "%d"));

        utility::print(        //
            " -- verbose: " +  //
            utility::to_string(this->verbose, "%d"));

        utility::print(                       //
            " -- is_enabled_write_trend: " +  //
            utility::to_string(this->is_enabled_write_trend, "%d"));

        utility::print(                                    //
            " -- is_enabled_store_feasible_solutions: " +  //
            utility::to_string(this->is_enabled_store_feasible_solutions,
                               "%d"));

        utility::print(                            //
            " -- feasible_solutions_capacity: " +  //
            utility::to_string(this->feasible_solutions_capacity, "%d"));

        utility::print(                            //
            " -- lagrange_dual.iteration_max: " +  //
            utility::to_string(this->lagrange_dual.iteration_max, "%d"));

        utility::print(  //
            " -- lagrange_dual.time_max: " +
            utility::to_string(this->lagrange_dual.time_max, "%f"));

        utility::print(                          //
            " -- lagrange_dual.time_offset: " +  //
            utility::to_string(this->lagrange_dual.time_offset, "%f"));

        utility::print(                                    //
            " -- lagrange_dual.step_size_extend_rate: " +  //
            utility::to_string(this->lagrange_dual.step_size_extend_rate,
                               "%f"));

        utility::print(                                    //
            " -- lagrange_dual.step_size_reduce_rate: " +  //
            utility::to_string(this->lagrange_dual.step_size_reduce_rate,
                               "%f"));

        utility::print(                        //
            " -- lagrange_dual.tolerance: " +  //
            utility::to_string(this->lagrange_dual.tolerance, "%f"));

        utility::print(                         //
            " -- lagrange_dual.queue_size: " +  //
            utility::to_string(this->lagrange_dual.queue_size, "%d"));

        utility::print(                           //
            " -- lagrange_dual.log_interval: " +  //
            utility::to_string(this->lagrange_dual.log_interval, "%d"));

        utility::print(                           //
            " -- local_search.iteration_max: " +  //
            utility::to_string(this->local_search.iteration_max, "%d"));

        utility::print(  //
            " -- local_search.time_max: " +
            utility::to_string(this->local_search.time_max, "%f"));

        utility::print(                         //
            " -- local_search.time_offset: " +  //
            utility::to_string(this->local_search.time_offset, "%f"));

        utility::print(                          //
            " -- local_search.log_interval: " +  //
            utility::to_string(this->local_search.log_interval, "%d"));

        utility::print(                  //
            " -- local_search.seed: " +  //
            utility::to_string(this->local_search.seed, "%d"));

        utility::print(                          //
            " -- tabu_search.iteration_max: " +  //
            utility::to_string(this->tabu_search.iteration_max, "%d"));

        utility::print(                     //
            " -- tabu_search.time_max: " +  //
            utility::to_string(this->tabu_search.time_max, "%f"));

        utility::print(                        //
            " -- tabu_search.time_offset: " +  //
            utility::to_string(this->tabu_search.time_offset, "%f"));

        utility::print(                         //
            " -- tabu_search.log_interval: " +  //
            utility::to_string(this->tabu_search.log_interval, "%d"));

        utility::print(  //
            " -- tabu_search.initial_tabu_tenure: " +
            utility::to_string(this->tabu_search.initial_tabu_tenure, "%d"));

        utility::print(  //
            " -- tabu_search.tabu_tenure_randomize_rate: " +
            utility::to_string(this->tabu_search.tabu_tenure_randomize_rate,
                               "%f"));

        utility::print(  //
            " -- tabu_search.initial_modification_fixed_rate: " +
            utility::to_string(
                this->tabu_search.initial_modification_fixed_rate, "%f"));

        utility::print(  //
            " -- tabu_search.initial_modification_randomize_rate: " +
            utility::to_string(
                this->tabu_search.initial_modification_randomize_rate, "%f"));

        utility::print(                      //
            " -- tabu_search.tabu_mode: " +  //
            utility::to_string(this->tabu_search.tabu_mode, "%d"));

        utility::print(                               //
            " -- tabu_search.move_preserve_rate: " +  //
            utility::to_string(this->tabu_search.move_preserve_rate, "%f"));

        utility::print(                                          //
            " -- tabu_search.frequency_penalty_coefficient: " +  //
            utility::to_string(this->tabu_search.frequency_penalty_coefficient,
                               "%f"));

        utility::print(                                           //
            " -- tabu_search.lagrangian_penalty_coefficient: " +  //
            utility::to_string(this->tabu_search.lagrangian_penalty_coefficient,
                               "%f"));

        utility::print(                                   //
            " -- tabu_search.pruning_rate_threshold: " +  //
            utility::to_string(this->tabu_search.pruning_rate_threshold, "%f"));

        utility::print(                               //
            " -- tabu_search.is_enabled_shuffle: " +  //
            utility::to_string(this->tabu_search.is_enabled_shuffle, "%d"));

        utility::print(                                    //
            " -- tabu_search.is_enabled_move_curtail: " +  //
            utility::to_string(this->tabu_search.is_enabled_move_curtail,
                               "%d"));

        utility::print(                                       //
            " -- tabu_search.is_enabled_automatic_break: " +  //
            utility::to_string(this->tabu_search.is_enabled_automatic_break,
                               "%d"));

        utility::print(  //
            " -- tabu_search.is_enabled_automatic_tabu_tenure_adjustment: " +  //
            utility::to_string(
                this->tabu_search.is_enabled_automatic_tabu_tenure_adjustment,
                "%d"));

        utility::print(                                                      //
            " -- tabu_search.is_enabled_automatic_iteration_adjustment: " +  //
            utility::to_string(
                this->tabu_search.is_enabled_automatic_iteration_adjustment,
                "%d"));

        utility::print(                                            //
            " -- tabu_search.is_enabled_initial_modification: " +  //
            utility::to_string(
                this->tabu_search.is_enabled_initial_modification, "%d"));

        utility::print(                                               //
            " -- tabu_search.intensity_increase_count_threshold: " +  //
            utility::to_string(
                this->tabu_search.intensity_increase_count_threshold, "%d"));

        utility::print(                                               //
            " -- tabu_search.intensity_decrease_count_threshold: " +  //
            utility::to_string(
                this->tabu_search.intensity_decrease_count_threshold, "%d"));

        utility::print(                                    //
            " -- tabu_search.iteration_increase_rate: " +  //
            utility::to_string(this->tabu_search.iteration_increase_rate,
                               "%f"));

        utility::print(                                    //
            " -- tabu_search.iteration_decrease_rate: " +  //
            utility::to_string(this->tabu_search.iteration_decrease_rate,
                               "%f"));

        utility::print(                                            //
            " -- tabu_search.ignore_tabu_if_global_incumbent: " +  //
            utility::to_string(
                this->tabu_search.ignore_tabu_if_global_incumbent, "%d"));

        utility::print(                                           //
            " -- tabu_search.number_of_initial_modification: " +  //
            utility::to_string(this->tabu_search.number_of_initial_modification,
                               "%d"));

        utility::print(               //
            " -- tabu_search.seed: "  //
            + utility::to_string(this->tabu_search.seed, "%d"));
    }

   private:
    /**************************************************************************/
    inline static bool read_json(double *a_parameter, const std::string &a_NAME,
                                 const utility::json::JsonObject &a_JSON) {
        if (a_JSON.find(a_NAME)) {
            if (a_JSON.at(a_NAME).type() == typeid(int)) {
                *a_parameter = a_JSON.get<int>(a_NAME);
            } else {
                *a_parameter = a_JSON.get<double>(a_NAME);
            }
            return true;
        }
        return false;
    }

    /**************************************************************************/
    inline static bool read_json(
        chain_move_reduce_mode::ChainMoveReduceMode *a_parameter,
        const std::string &a_NAME, const utility::json::JsonObject &a_JSON) {
        if (a_JSON.find(a_NAME)) {
            *a_parameter =
                static_cast<chain_move_reduce_mode::ChainMoveReduceMode>(
                    a_JSON.get<int>(a_NAME));
            return true;
        }
        return false;
    }

    /**************************************************************************/
    inline static bool read_json(selection_mode::SelectionMode *  a_parameter,
                                 const std::string &              a_NAME,
                                 const utility::json::JsonObject &a_JSON) {
        if (a_JSON.find(a_NAME)) {
            *a_parameter = static_cast<selection_mode::SelectionMode>(
                a_JSON.get<int>(a_NAME));
            return true;
        }
        return false;
    }

    /**************************************************************************/
    inline static bool read_json(
        improvability_screening_mode::ImprovabilityScreeningMode *a_parameter,
        const std::string &a_NAME, const utility::json::JsonObject &a_JSON) {
        if (a_JSON.find(a_NAME)) {
            *a_parameter = static_cast<
                improvability_screening_mode::ImprovabilityScreeningMode>(
                a_JSON.get<int>(a_NAME));
            return true;
        }
        return false;
    }

    /**************************************************************************/
    inline static bool read_json(tabu_mode::TabuMode *            a_parameter,
                                 const std::string &              a_NAME,
                                 const utility::json::JsonObject &a_JSON) {
        if (a_JSON.find(a_NAME)) {
            *a_parameter =
                static_cast<tabu_mode::TabuMode>(a_JSON.get<int>(a_NAME));
            return true;
        }
        return false;
    }

    /**************************************************************************/
    template <class T>
    inline static bool read_json(T *a_parameter, const std::string &a_NAME,
                                 const utility::json::JsonObject &a_JSON) {
        if (a_JSON.find(a_NAME)) {
            *a_parameter = a_JSON.get<T>(a_NAME);
            return true;
        }
        return false;
    }

   public:
    /******************************************************************************/
    inline void read(const std::string &a_FILE_NAME) {
        auto   option_object = utility::json::read_json_object(a_FILE_NAME);
        Option option;

        /**************************************************************************/
        /// global
        /**************************************************************************/
        /// iteration_max
        read_json(&option.iteration_max,  //
                  "iteration_max",        //
                  option_object);

        /**************************************************************************/
        /// time_offset
        read_json(&option.time_offset,  //
                  "time_offset",        //
                  option_object);

        /**************************************************************************/
        /// time_max
        read_json(&option.time_max,  //
                  "time_max",        //
                  option_object);

        /**************************************************************************/
        /// penalty_coefficient_relaxing_rate
        read_json(&option.penalty_coefficient_relaxing_rate,  //
                  "penalty_coefficient_relaxing_rate",        //
                  option_object);

        /**************************************************************************/
        /// penalty_coefficient_tightening_rate
        read_json(&option.penalty_coefficient_tightening_rate,  //
                  "penalty_coefficient_tightening_rate",        //
                  option_object);

        /**************************************************************************/
        /// penalty_coefficient_updating_balance
        read_json(&option.penalty_coefficient_updating_balance,  //
                  "penalty_coefficient_updating_balance",        //
                  option_object);

        /**************************************************************************/
        /// initial_penalty_coefficient
        read_json(&option.initial_penalty_coefficient,  //
                  "initial_penalty_coefficient",        //
                  option_object);

        /**************************************************************************/
        /// is_enabled_lagrange_dual
        read_json(&option.is_enabled_lagrange_dual,  //
                  "is_enabled_lagrange_dual",        //
                  option_object);

        /**************************************************************************/
        /// is_enabled_local_search
        read_json(&option.is_enabled_local_search,  //
                  "is_enabled_local_search",        //
                  option_object);

        /**************************************************************************/
        /// is_enabled_grouping_penalty_coefficient
        read_json(&option.is_enabled_grouping_penalty_coefficient,  //
                  "is_enabled_grouping_penalty_coefficient",        //
                  option_object);

        /**************************************************************************/
        /// is_enabled_presolve
        read_json(&option.is_enabled_presolve,  //
                  "is_enabled_presolve",        //
                  option_object);
        /**************************************************************************/
        /// is_enabled_initial_value_correction
        read_json(&option.is_enabled_initial_value_correction,  //
                  "is_enabled_initial_value_correction",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_parallel_evaluation
        read_json(&option.is_enabled_parallel_evaluation,  //
                  "is_enabled_parallel_evaluation",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_parallel_neighborhood_update
        read_json(&option.is_enabled_parallel_neighborhood_update,  //
                  "is_enabled_parallel_neighborhood_update",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_binary_move
        read_json(&option.is_enabled_binary_move,  //
                  "is_enabled_binary_move",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_integer_move
        read_json(&option.is_enabled_integer_move,  //
                  "is_enabled_integer_move",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_aggregation_move
        read_json(&option.is_enabled_aggregation_move,  //
                  "is_enabled_aggregation_move",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_precedence_move
        read_json(&option.is_enabled_precedence_move,  //
                  "is_enabled_precedence_move",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_variable_bound_move
        read_json(&option.is_enabled_variable_bound_move,  //
                  "is_enabled_variable_bound_move",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_soft_selection_move
        read_json(&option.is_enabled_soft_selection_move,  //
                  "is_enabled_soft_selection_move",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_chain_move
        read_json(&option.is_enabled_chain_move,  //
                  "is_enabled_chain_move",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_chain_move
        read_json(&option.is_enabled_two_flip_move,  //
                  "is_enabled_two_flip_move",        //
                  option_object);

        /**********************************************************************/
        /// is_enabled_user_defined_move
        read_json(&option.is_enabled_user_defined_move,  //
                  "is_enabled_user_defined_move",        //
                  option_object);

        /**********************************************************************/
        /// chain_move_capacity
        read_json(&option.chain_move_capacity,  //
                  "chain_move_capacity",        //
                  option_object);

        /**********************************************************************/
        /// chain_move_reduce_mode
        read_json(&option.chain_move_reduce_mode,  //
                  "chain_move_reduce_mode",        //
                  option_object);

        /**********************************************************************/
        /// chain_move_overlap_rate_threshold
        read_json(&option.chain_move_overlap_rate_threshold,  //
                  "chain_move_overlap_rate_threshold",        //
                  option_object);

        /**************************************************************************/
        /// selection_mode
        read_json(&option.selection_mode,  //
                  "selection_mode",        //
                  option_object);

        /**************************************************************************/
        /// improvability_screening_mode
        read_json(&option.improvability_screening_mode,  //
                  "improvability_screening_mode",        //
                  option_object);

        /**************************************************************************/
        /// target_objective_value
        read_json(&option.target_objective_value,  //
                  "target_objective_value",        //
                  option_object);

        /**************************************************************************/
        /// seed
        read_json(&option.seed,  //
                  "seed",        //
                  option_object);
        /**************************************************************************/
        /// verbose
        read_json(&option.verbose,  //
                  "verbose",        //
                  option_object);
        /**************************************************************************/
        /// verbose
        read_json(&option.is_enabled_write_trend,  //
                  "is_enabled_write_trend",        //
                  option_object);

        /**************************************************************************/
        /// is_enabled_store_feasible_solutions
        read_json(&option.is_enabled_store_feasible_solutions,  //
                  "is_enabled_store_feasible_solutions",        //
                  option_object);

        /**************************************************************************/
        /// feasible_solutions_capacity
        read_json(&option.feasible_solutions_capacity,  //
                  "feasible_solutions_capacity",        //
                  option_object);

        /**************************************************************************/
        /// lagrange dual
        /**************************************************************************/
        utility::json::JsonObject option_object_lagrange_dual;

        bool has_lagrange_dual_option =
            read_json(&option_object_lagrange_dual,  //
                      "lagrange_dual",               //
                      option_object);
        if (has_lagrange_dual_option) {
            /**********************************************************************/
            /// lagrange_dual.iteration_max
            read_json(&option.lagrange_dual.iteration_max,  //
                      "iteration_max",                      //
                      option_object_lagrange_dual);

            /**********************************************************************/
            /// lagrange_dual.time_max
            read_json(&option.lagrange_dual.time_max,  //
                      "time_max",                      //
                      option_object_lagrange_dual);

            /**********************************************************************/
            /// lagrange_dual.time_offset
            read_json(&option.lagrange_dual.time_offset,  //
                      "time_offset",                      //
                      option_object_lagrange_dual);

            /**********************************************************************/
            /// lagrange_dual.step_size_extend_rate
            read_json(&option.lagrange_dual.step_size_extend_rate,  //
                      "step_size_extend_rate",                      //
                      option_object_lagrange_dual);

            /**********************************************************************/
            /// lagrange_dual.step_size_reduce_rate
            read_json(&option.lagrange_dual.step_size_reduce_rate,  //
                      "step_size_reduce_rate",                      //
                      option_object_lagrange_dual);

            /**********************************************************************/
            /// lagrange_dual.tolerance
            read_json(&option.lagrange_dual.tolerance,  //
                      "tolerance",                      //
                      option_object_lagrange_dual);

            /**********************************************************************/
            /// lagrange_dual.queue_size
            read_json(&option.lagrange_dual.queue_size,  //
                      "queue_size",                      //
                      option_object_lagrange_dual);

            /**********************************************************************/
            /// lagrange_dual.log_interval
            read_json(&option.lagrange_dual.log_interval,  //
                      "log_interval",                      //
                      option_object_lagrange_dual);
        }

        /**************************************************************************/
        /// local search
        /**************************************************************************/
        utility::json::JsonObject option_object_local_search;
        bool                      has_local_search_option =
            read_json(&option_object_local_search,  //
                      "local_search",               //
                      option_object);
        if (has_local_search_option) {
            /**********************************************************************/
            /// local_search.iteration_max
            read_json(&option.local_search.iteration_max,  //
                      "iteration_max",                     //
                      option_object_local_search);

            /**********************************************************************/
            /// local_search.time_max
            read_json(&option.local_search.time_max,  //
                      "time_max",                     //
                      option_object_local_search);

            /**********************************************************************/
            /// local_search.time_offset
            read_json(&option.local_search.time_offset,  //
                      "time_offset",                     //
                      option_object_local_search);

            /**********************************************************************/
            /// local_search.log_interval
            read_json(&option.local_search.log_interval,  //
                      "log_interval",                     //
                      option_object_local_search);

            /**********************************************************************/
            /// local_search.seed
            read_json(&option.local_search.seed,  //
                      "seed",                     //
                      option_object_local_search);
        }

        /**************************************************************************/
        /// tabu search
        /**************************************************************************/
        utility::json::JsonObject option_object_tabu_search;

        bool has_tabu_search_option = read_json(&option_object_tabu_search,  //
                                                "tabu_search",               //
                                                option_object);

        if (has_tabu_search_option) {
            /**********************************************************************/
            /// tabu_search.iteration_max
            read_json(&option.tabu_search.iteration_max,  //
                      "iteration_max",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.time_max
            read_json(&option.tabu_search.time_max,  //
                      "time_max",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.time_offset
            read_json(&option.tabu_search.time_offset,  //
                      "time_offset",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.log_interval
            read_json(&option.tabu_search.log_interval,  //
                      "log_interval",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.initial_tabu_tenure
            read_json(&option.tabu_search.initial_tabu_tenure,  //
                      "initial_tabu_tenure",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.tabu_tenure_randomize_rate
            read_json(&option.tabu_search.tabu_tenure_randomize_rate,  //
                      "tabu_tenure_randomize_rate",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.initial_modification_fixed_rate
            read_json(&option.tabu_search.initial_modification_fixed_rate,  //
                      "initial_modification_fixed_rate",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.initial_modification_randomize_rate
            read_json(
                &option.tabu_search.initial_modification_randomize_rate,  //
                "initial_modification_randomize_rate",                    //
                option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.tabu_mode
            read_json(&option.tabu_search.tabu_mode,  //
                      "tabu_mode",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.move_preserve_rate
            read_json(&option.tabu_search.move_preserve_rate,  //
                      "move_preserve_rate",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.frequency_penalty_coefficient
            read_json(&option.tabu_search.frequency_penalty_coefficient,  //
                      "frequency_penalty_coefficient",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.lagrangian_penalty_coefficient
            read_json(&option.tabu_search.lagrangian_penalty_coefficient,  //
                      "lagrangian_penalty_coefficient",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.pruning_rate_threshold
            read_json(&option.tabu_search.pruning_rate_threshold,  //
                      "pruning_rate_threshold",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.is_enabled_shuffle
            read_json(&option.tabu_search.is_enabled_shuffle,  //
                      "is_enabled_shuffle",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.is_enabled_move_curtail
            read_json(&option.tabu_search.is_enabled_move_curtail,  //
                      "is_enabled_move_curtail",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.is_enabled_automatic_break
            read_json(&option.tabu_search.is_enabled_automatic_break,  //
                      "is_enabled_automatic_break",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.is_enabled_automatic_tabu_tenure_adjustment
            read_json(&option.tabu_search
                           .is_enabled_automatic_tabu_tenure_adjustment,  //
                      "is_enabled_automatic_tabu_tenure_adjustment",      //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.is_enabled_automatic_iteration_adjustment
            read_json(&option.tabu_search
                           .is_enabled_automatic_iteration_adjustment,  //
                      "is_enabled_automatic_iteration_adjustment",      //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.is_enabled_initial_modification
            read_json(&option.tabu_search.is_enabled_initial_modification,  //
                      "is_enabled_initial_modification",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.intensity_increase_count_threshold
            read_json(
                &option.tabu_search.intensity_increase_count_threshold,  //
                "intensity_increase_count_threshold",                    //
                option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.intensity_decrease_count_threshold
            read_json(
                &option.tabu_search.intensity_decrease_count_threshold,  //
                "intensity_decrease_count_threshold",                    //
                option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.iteration_increase_rate
            read_json(&option.tabu_search.iteration_increase_rate,  //
                      "iteration_increase_rate",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.iteration_decrease_rate
            read_json(&option.tabu_search.iteration_decrease_rate,  //
                      "iteration_decrease_rate",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.ignore_tabu_if_global_incumbent
            read_json(&option.tabu_search.ignore_tabu_if_global_incumbent,  //
                      "ignore_tabu_if_global_incumbent",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.number_of_initial_modification
            read_json(&option.tabu_search.number_of_initial_modification,  //
                      "number_of_initial_modification",                    //
                      option_object_tabu_search);

            /**********************************************************************/
            /// tabu_search.seed
            read_json(&option.tabu_search.seed,  //
                      "seed",                    //
                      option_object_tabu_search);
        }
        *this = option;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/