/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPB_OPB_METADATA_H__
#define PRINTEMPS_OPB_OPB_METADATA_H__

namespace printemps::opb {
/*****************************************************************************/
struct OPBMetadata {
    int number_of_variables;
    int number_of_constraints;
    int number_of_equals;
    int number_of_products;
    int intsize;
    int sizeproduct;

    /**
     * The following members are for WBO.
     */
    int number_of_soft_constraints;
    int mincost;
    int maxcost;
    int sumcost;

    /*************************************************************************/
    OPBMetadata(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->number_of_variables   = 0;
        this->number_of_constraints = 0;
        this->number_of_equals      = 0;
        this->number_of_products    = 0;
        this->intsize               = 0;
        this->sizeproduct           = 0;

        this->number_of_soft_constraints = 0;
        this->mincost                    = 0;
        this->maxcost                    = 0;
        this->sumcost                    = 0;
    }
};
}  // namespace printemps::opb
#endif
/*****************************************************************************/
// END
/*****************************************************************************/