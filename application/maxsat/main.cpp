/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#define _PRINTEMPS_STYLING
#include <printemps.h>

int main(const int argc, const char *argv[]) {
    printemps::standalone::MaxSATStandalone app;
    app.setup(argc, argv);
    return app.solve();
}
/*****************************************************************************/
// END
/*****************************************************************************/
