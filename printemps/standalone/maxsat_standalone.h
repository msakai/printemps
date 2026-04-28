/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_STANDALONE_MAXSAT_STANDALONE_H__
#define PRINTEMPS_STANDALONE_MAXSAT_STANDALONE_H__

namespace printemps::standalone {
/*****************************************************************************/
class MaxSATStandalone {
   private:
    wcnf::WCNF          m_wcnf;
    model::IPModel      m_model;
    option::Option      m_option;
    utility::TimeKeeper m_time_keeper;

    std::string m_instance_file_name;
    double      m_timeout_seconds;

    /**
     * Best feasible objective value emitted so far. Initialized to +infinity
     * so that the first feasible incumbent always passes the improvement
     * check. Mutated only from the solver thread (callback runs serially).
     */
    double      m_best_objective;
    bool        m_have_emitted_solution;
    std::string m_last_v_line;

   public:
    /*************************************************************************/
    MaxSATStandalone(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_wcnf.initialize();
        m_model.initialize();
        m_option.initialize();
        m_time_keeper.initialize();

        m_instance_file_name.clear();
        m_timeout_seconds       = 0.0;
        m_best_objective        = std::numeric_limits<double>::infinity();
        m_have_emitted_solution = false;
        m_last_v_line.clear();
    }

    /*************************************************************************/
    inline static void print_usage(void) {
        std::cout << std::endl;
        std::cout << "PRINTEMPS " + constant::VERSION + " (" +
                         constant::PROJECT_URL + ") -- MaxSAT Anytime mode"
                  << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: ./printemps-maxsat <input.wcnf> <timeout_seconds>"
                  << std::endl;
    }

    /*************************************************************************/
    inline void setup(const int argc, const char *argv[]) {
        if (argc < 3) {
            print_usage();
            std::exit(1);
        }

        m_time_keeper.set_start_time();

        m_instance_file_name = argv[1];
        try {
            m_timeout_seconds = std::stod(argv[2]);
        } catch (const std::exception &) {
            std::cerr << "c invalid timeout argument: " << argv[2] << std::endl;
            std::exit(1);
        }
        if (m_timeout_seconds <= 0.0) {
            std::cerr << "c timeout must be positive: " << argv[2] << std::endl;
            std::exit(1);
        }

        /**
         * Force PRINTEMPS to be silent so that only c/o/v/s lines appear on
         * stdout. Force single-threaded execution per MSE rules.
         */
        m_option.output.verbose                            = option::verbose::Off;
        m_option.parallel.number_of_threads_move_evaluation = 1;
        m_option.parallel.number_of_threads_move_update     = 1;
#ifdef _OPENMP
        omp_set_num_threads(1);
#endif

        /**
         * Reserve a small wall-clock margin so that we have time to flush the
         * final s/o/v lines before SIGTERM->SIGKILL. The margin matters less
         * for the long timeouts but is critical at 60 s.
         */
        const double SAFETY_MARGIN_SECONDS = 1.0;
        m_option.general.time_max =
            std::max(m_timeout_seconds - SAFETY_MARGIN_SECONDS,
                     m_timeout_seconds * 0.95);

        /**
         * Improve PRINTEMPS' behaviour on Boolean disjunctive constraints.
         */
        m_option.neighborhood.is_enabled_two_flip_move = true;

        /**
         * Make stdout line-buffered (effectively unitbuf for cout) so that
         * each o/v line reaches the parent before SIGKILL strikes.
         */
        std::cout << std::unitbuf;

        /**
         * Parse the WCNF instance and import it into the IPModel.
         */
        m_wcnf.read_wcnf(m_instance_file_name);
        m_model.set_name(utility::base_name(m_instance_file_name));
        m_model.import_wcnf(m_wcnf);

        /**
         * Install signal handlers reusing the standalone-mode interrupt flag.
         */
        signal(SIGINT, interrupt_handler);
        signal(SIGTERM, interrupt_handler);
#ifndef _WIN32
        signal(SIGXCPU, interrupt_handler);
#endif
    }

    /*************************************************************************/
    inline void on_incumbent(solver::IPGlobalState *a_GLOBAL_STATE) {
        if (a_GLOBAL_STATE == nullptr) {
            return;
        }
        const auto &HOLDER = a_GLOBAL_STATE->incumbent_holder;
        if (!HOLDER.is_found_feasible_solution()) {
            return;
        }
        const double CURRENT = HOLDER.feasible_incumbent_objective();
        if (CURRENT + constant::EPSILON >= m_best_objective) {
            return;
        }
        m_best_objective = CURRENT;
        emit_solution(HOLDER.feasible_incumbent_solution());
    }

    /*************************************************************************/
    inline void emit_solution(const solution::IPDenseSolution &a_SOLUTION) {
        /**
         * The first variable proxy named "variables" holds x_1..x_n in
         * 1-indexed order; the soft-clause slacks live in subsequent proxies
         * and are excluded from the v-line.
         */
        std::string v_line;
        if (!a_SOLUTION.variable_value_proxies.empty()) {
            const auto &PROXY  = a_SOLUTION.variable_value_proxies.front();
            const auto &VALUES = PROXY.flat_indexed_values();
            const int   N      = static_cast<int>(VALUES.size());
            v_line.reserve(N);
            for (auto i = 0; i < N; i++) {
                v_line.push_back(VALUES[i] != 0 ? '1' : '0');
            }
        }

        const long long COST = static_cast<long long>(
            std::llround(static_cast<double>(a_SOLUTION.objective)));
        std::cout << "o " << COST << "\n";
        std::cout << "v " << v_line << "\n";
        std::cout.flush();

        m_last_v_line          = std::move(v_line);
        m_have_emitted_solution = true;
    }

    /*************************************************************************/
    inline int solve(void) {
        solver::IPSolver solver;
        solver.setup(&m_model, m_option, m_time_keeper);
        solver.set_check_interrupt([]() { return interrupted; });
        solver.set_callback(
            [this](solver::IPGlobalState *g) { this->on_incumbent(g); });

        const auto RESULT = solver.solve();

        /**
         * If the final feasible incumbent is strictly better than what we
         * streamed during the search, emit a final pair of o/v lines.
         */
        const auto &SOLUTION = RESULT.solution;
        if (SOLUTION.is_feasible()) {
            const double FINAL_COST =
                static_cast<double>(SOLUTION.objective());
            if (FINAL_COST + constant::EPSILON < m_best_objective) {
                m_best_objective = FINAL_COST;
                std::string v_line;
                const auto &PROXY  = SOLUTION.variables("variables");
                const auto &VALUES = PROXY.flat_indexed_values();
                const int   N      = static_cast<int>(VALUES.size());
                v_line.reserve(N);
                for (auto i = 0; i < N; i++) {
                    v_line.push_back(VALUES[i] != 0 ? '1' : '0');
                }
                const long long COST = static_cast<long long>(
                    std::llround(FINAL_COST));
                std::cout << "o " << COST << "\n";
                std::cout << "v " << v_line << "\n";
                std::cout.flush();
                m_last_v_line          = std::move(v_line);
                m_have_emitted_solution = true;
            }
        }

        /**
         * MSE 2026 status / exit code mapping. PRINTEMPS does not prove
         * optimality nor unsatisfiability of hard clauses, so we emit
         * SATISFIABLE on any feasible incumbent and UNKNOWN otherwise.
         */
        if (m_have_emitted_solution) {
            std::cout << "s SATISFIABLE" << std::endl;
            return 10;
        } else {
            std::cout << "s UNKNOWN" << std::endl;
            return 0;
        }
    }
};
}  // namespace printemps::standalone
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
