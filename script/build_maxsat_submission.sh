#!/usr/bin/env bash
# Build the MaxSAT Evaluation 2026 anytime-track submission package.
#
# Output layout (under submission/maxsat-2026/):
#   bin/printemps-maxsat       statically linked Linux binary
#   bin/run_ANYTIME-W          launch script for weighted track
#   bin/run_ANYTIME-UW         launch script for unweighted track
#   code/printemps-maxsat.tar.gz  source archive (git archive HEAD)
#   doc/printemps-maxsat-2026.pdf  solver description (placeholder)
#
# Requires the docker image defined in docker/Dockerfile (Ubuntu 22.04 +
# build-essential) so the resulting binary uses an old-enough glibc to run on
# the MSE evaluation infrastructure.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SUBMISSION_DIR="${REPO_ROOT}/submission/maxsat-2026"

cd "${REPO_ROOT}"

echo "[1/4] Cleaning previous build artifacts ..."
rm -rf "${REPO_ROOT}/build/application"
rm -f  "${SUBMISSION_DIR}/bin/printemps-maxsat"
rm -f  "${SUBMISSION_DIR}/code/printemps-maxsat.tar.gz"

echo "[2/4] Building printemps-maxsat statically inside docker ..."
mkdir -p "${SUBMISSION_DIR}/bin" "${SUBMISSION_DIR}/code" "${SUBMISSION_DIR}/doc"

docker compose run --rm develop bash -c '
    set -euo pipefail
    cd /home/work
    make -f makefile/Makefile.external
    make -f makefile/Makefile.application STATIC=ON
'

cp "${REPO_ROOT}/build/application/Release/printemps-maxsat" \
   "${SUBMISSION_DIR}/bin/printemps-maxsat"
chmod +x "${SUBMISSION_DIR}/bin/printemps-maxsat"

echo "[3/4] Generating run scripts ..."
for TRACK in ANYTIME-W ANYTIME-UW; do
    cat > "${SUBMISSION_DIR}/bin/run_${TRACK}" <<'EOF'
#!/bin/sh
exec "$(dirname "$0")/printemps-maxsat" "$1" "$2"
EOF
    chmod +x "${SUBMISSION_DIR}/bin/run_${TRACK}"
done

echo "[4/4] Archiving source tree ..."
git -C "${REPO_ROOT}" archive --format=tar.gz \
    --prefix=printemps-maxsat/ \
    -o "${SUBMISSION_DIR}/code/printemps-maxsat.tar.gz" HEAD

echo
echo "Submission package built under: ${SUBMISSION_DIR}"
ls -l "${SUBMISSION_DIR}/bin"
echo
echo "Remember to drop the solver description PDF into ${SUBMISSION_DIR}/doc/"
echo "before zipping and submitting."
