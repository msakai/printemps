/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPB_TOP_COST_H__
#define PRINTEMPS_OPB_TOP_COST_H__

namespace printemps::opb {
/*****************************************************************************/
struct OPBTopCost {
    bool is_defined;
    int  value;

    /*************************************************************************/
    OPBTopCost(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->is_defined = false;
        this->value      = std::numeric_limits<int>::max();
    }
};
}  // namespace printemps::opb
#endif
/*****************************************************************************/
// END
/*****************************************************************************/