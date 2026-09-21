#!/usr/bin/env bash
# Overnight benchmark runner for the HODLR work.
#
# Runs the full sweep of python/test/benchmark_hmat_hodlr.py twice:
#   1. against the current 'hodlr' branch install  -> results_branch.csv
#   2. against the PR #3272 (hmat-oss HODLR) install -> results_pr3272.csv
# then combines both CSVs into results_combined.csv with a 'build' column.
#
# Usage:
#   ./run_hodlr_bench.sh                 # full default sweep
#   BENCH_ARGS="--dims 1,2" ./run_hodlr_bench.sh   # reduced sweep
#   FORCE=1 ./run_hodlr_bench.sh         # run even if the machine is busy
set -u

# ---- configuration --------------------------------------------------------
BRANCH_PY="/home/regis/openturns-nomade/build/install/lib64/python3.13/site-packages"
PR_PY="/home/regis/openturns-pr-3272/build/install/lib64/python3.13/site-packages"
BENCH="/home/regis/openturns-nomade/python/test/benchmark_hmat_hodlr.py"
OUTDIR="/home/regis/openturns-nomade/python/test/results"

# Full default sweep (keep in sync with the script defaults); stop at n=20000
SWEEP="--dims 1,2,3,4 --corr 0.1,0.01 --cov matern,exponential --methods LAPACK,HMAT,HMAT_HODLR,HODLR --max-n 20000"
BENCH_ARGS="${BENCH_ARGS:-$SWEEP}"

# abort when the (1-min) load average is above this on a busy machine,
# since it would bias the timings; FORCE=1 skips the guard
MAX_LOAD="${MAX_LOAD:-4}"
ABORT_IF_LOADED="${ABORT_IF_LOADED:-1}"

# ---- sanity checks --------------------------------------------------------
for py in "$BRANCH_PY" "$PR_PY"; do
    if [ ! -f "$py/openturns/__init__.py" ]; then
        echo "ERROR: openturns not installed in $py" >&2
        exit 2
    fi
done

if [ "${FORCE:-0}" != "1" ] && [ "$ABORT_IF_LOADED" = "1" ]; then
    LOAD=$(awk '{print $1}' /proc/loadavg)
    if awk "BEGIN { exit !($LOAD > $MAX_LOAD) }"; then
        echo "ABORT: 1-min load average is $LOAD (max $MAX_LOAD);" \
             "timings would be biased, retry when the machine is idle." >&2
        echo "       override with FORCE=1 $0" >&2
        exit 3
    fi
fi

mkdir -p "$OUTDIR"
rm -f "$OUTDIR"/results_branch.csv "$OUTDIR"/results_pr3272.csv \
      "$OUTDIR"/results_combined.csv

run_bench() {
    local name="$1" py="$2" csv="$3"
    echo
    echo "=== [$(date '+%F %T')] benchmark against the $name build ==="
    PYTHONPATH="$py" python3 "$BENCH" $BENCH_ARGS --csv "$csv" \
        | tee "$OUTDIR/run_$name.log"
    echo "    -> $csv written"
}

run_bench "branch"  "$BRANCH_PY" "$OUTDIR/results_branch.csv"
run_bench "pr3272"  "$PR_PY"     "$OUTDIR/results_pr3272.csv"

# ---- combine the two result sets ------------------------------------------
echo
echo "=== [$(date '+%F %T')] combining results ==="
python3 - "$OUTDIR" <<'PY'
import os
import sys

outdir = sys.argv[1]
cols = ["dim", "cov", "corr", "method", "n_total",
        "t_first", "t_second", "t_kl", "t_fitter", "t_regression"]
rows = []
for build, fname in [("hodlr-branch", "results_branch.csv"),
                     ("pr3272", "results_pr3272.csv")]:
    with open(os.path.join(outdir, fname), encoding="utf-8") as f:
        header = f.readline().strip().split(",")
        for line in f:
            line = line.strip()
            if not line:
                continue
            values = line.split(",")
            row = dict(zip(header, values))
            row["build"] = build
            rows.append(row)

with open(os.path.join(outdir, "results_combined.csv"),
          "w", encoding="utf-8") as f:
    f.write(",".join(["build"] + cols) + "\n")
    for row in rows:
        f.write(",".join([row["build"]] + [row[c] for c in cols]) + "\n")

nna = sum(1 for r in rows if all(r[c] in ("", "None") for c in cols[5:]))
print(f"combined {len(rows)} rows -> {outdir}/results_combined.csv "
      f"({nna} fully-failed rows)")
PY

echo
echo "=== [$(date '+%F %T')] all done ==="
ls -la "$OUTDIR"/results_*.csv