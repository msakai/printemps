# maxsat_evaluation_solver — MaxSAT Evaluation 2026 Anytime Track Submission

This directory holds the submission package for the MaxSAT Evaluation 2026
**Main Tracks for Anytime Solvers** (ANYTIME-W and ANYTIME-UW). The solver
wraps the open-source [PRINTEMPS](https://snowberryfield.github.io/printemps/)
metaheuristic IP solver and feeds it WCNF instances by linearising each
clause into a 0-1 inequality.

## Layout

```
bin/
  maxsat_evaluation_solver  statically linked Linux binary
  run_ANYTIME-W             launch script for the weighted track
  run_ANYTIME-UW            launch script for the unweighted track
code/
  printemps-maxsat.tar.gz   source archive (git archive HEAD)
doc/
  printemps-maxsat-2026.pdf 1-2 page solver description (IEEE format)
```

## Reproducing the build

The binary is produced inside the project's docker development environment
(Ubuntu 22.04, gcc) so that the resulting glibc dependency is old enough for
the MSE evaluation infrastructure.

```sh
# from repo root
script/build_maxsat_submission.sh
```

This:

1. wipes any previous `build/extra` artefacts;
2. fetches and builds googletest (no-op if already built);
3. compiles `maxsat_evaluation_solver` with `STATIC=ON` inside the docker
   compose service `develop`;
4. copies the binary plus `run_ANYTIME-{W,UW}` shell scripts to `bin/`;
5. archives the source via `git archive HEAD` to `code/`.

After the script completes, drop the solver-description PDF into `doc/` and
zip the directory for submission.

## Local sanity check

```sh
# unweighted, 60 second budget
bin/run_ANYTIME-UW <some_instance.wcnf> 60
# expected output:
#   o <cost>           (one per improving incumbent)
#   v <bitstring>      (one per improving incumbent)
#   s SATISFIABLE      (final, exit code 10)
```

## Notes

- The binary must run on a **single CPU core**. PRINTEMPS is built with
  OpenMP, but the MaxSAT entry point sets `omp_set_num_threads(1)` and
  `option.parallel.number_of_threads_*=1` before launching the solver.
- **SIGTERM** is caught and made to flush the best-so-far before
  termination. Stdout is set to unit-buffered so that o/v lines are not
  lost in buffer when the harness sends SIGKILL after SIGTERM.
- Status reporting is conservative: `s SATISFIABLE` whenever a feasible
  incumbent has been emitted, `s UNKNOWN` otherwise. PRINTEMPS does not
  prove optimality, so `s OPTIMUM FOUND` is never emitted. The wrapper
  also catches `printemps::model::InfeasibleError` from preprocess and
  reports `s UNSATISFIABLE` (exit 20) when the hard clauses are detected
  to be infeasible during variable-bound propagation.
- Soft-clause weights up to ~2^53 are exactly representable in the
  internal `double`-typed objective; weight sums beyond that incur
  precision loss but do not affect feasibility.
