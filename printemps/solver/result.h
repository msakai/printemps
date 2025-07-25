/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_RESULT_H__
#define PRINTEMPS_SOLVER_RESULT_H__

namespace printemps::solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Result {
    solution::NamedSolution<T_Variable, T_Expression> solution;
    Status<T_Variable, T_Expression>                  status;
    solution::SolutionArchive<T_Variable, T_Expression>
        feasible_solution_archive;

    /*************************************************************************/
    Result(solution::NamedSolution<T_Variable, T_Expression> &a_SOLUTION,
           Status<T_Variable, T_Expression>                  &a_STATUS,
           solution::SolutionArchive<T_Variable, T_Expression>
               &a_FEASIBLE_SOLUTION_ARCHIVE)
        : solution(a_SOLUTION),
          status(a_STATUS),
          feasible_solution_archive(a_FEASIBLE_SOLUTION_ARCHIVE) {
        /// nothing to do
    }

    /*************************************************************************/
    Result(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->solution.initialize();
        this->status.initialize();
        this->feasible_solution_archive.initialize();
    }
};
using IPResult = Result<int, double>;
}  // namespace printemps::solver

#endif
/*****************************************************************************/
// END
/*****************************************************************************/