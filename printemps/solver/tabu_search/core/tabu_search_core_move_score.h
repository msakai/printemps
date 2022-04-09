/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_MOVE_SCORE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_MOVE_SCORE_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace core {
/*****************************************************************************/
struct TabuSearchCoreMoveScore {
    bool   is_permissible     = false;
    double frequency_penalty  = 0.0;
    double lagrangian_penalty = 0.0;
};
}  // namespace core
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/