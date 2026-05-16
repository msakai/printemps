# maxsat_evaluation_solver: solver description (outline)

This is a placeholder outline for the 1-2 page IEEE-format PDF required by
the MaxSAT Evaluation 2026 anytime-track submission. Replace this file with
the actual `printemps-maxsat-2026.pdf` before zipping the submission.

## Suggested section structure

1. **Overview** — `maxsat_evaluation_solver` is a thin MaxSAT-track wrapper
   around PRINTEMPS, an open-source C++17 metaheuristic solver for general
   integer linear programming developed by Yuji Koguma. PRINTEMPS combines a
   tabu-search outer loop with PDLP / Lagrange-dual / local-search
   warm-start phases.

2. **WCNF -> 0-1 IP encoding** — each clause `(l_1 v ... v l_k)` is
   linearised as
   `sum_{positive lit} x_i - sum_{negative lit} x_j >= 1 - |N|`,
   where `|N|` is the number of negative literals. Hard clauses become
   constraints directly. Each soft clause of weight `w` introduces a
   binary slack `s` with the relaxed inequality
   `... + s >= 1 - |N|`, and `w*s` is added to the objective penalty,
   so that `s = 1` iff the clause is falsified.

3. **Anytime callback** — the solver's `set_callback` hook fires once per
   outer-loop iteration of the tabu search. The wrapper compares the
   current `feasible_incumbent_objective()` to the previously emitted
   value and, on improvement, writes a fresh `o <cost>` / `v <bitstring>`
   pair to stdout (line-buffered).

4. **Single-thread enforcement** — `omp_set_num_threads(1)` plus
   `option.parallel.number_of_threads_*=1`. Two-flip move is enabled by
   default.

5. **Signal handling** — `SIGINT` / `SIGTERM` / `SIGXCPU` set a namespace-
   local `interrupted` flag (mirroring the `pb_competition_2025_solver`
   pattern), polled by the tabu controller; final `o`/`v`/`s` lines are
   flushed on return.

6. **Failure modes** — `main()` catches
   `printemps::model::InfeasibleError` from the preprocess phase and
   reports `s UNSATISFIABLE` (exit 20); any other unexpected exception is
   reported as `s UNKNOWN`.

7. **Acknowledgements / citation** — cite the IEEE Access paper
   (Koguma 2024, doi 10.1109/ACCESS.2024.3361323) and the PRINTEMPS
   project page.
