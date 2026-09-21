"""Quick functional check for benchmark_hmat_hodlr.py.

Runs the same benchmark as benchmark_hmat_hodlr.py but with tiny matrix
sizes (about 50, 100 and 200 vertices) so every code path can be exercised
in a couple of minutes. Results are for error checking only, not timings.

Usage:
  python3 benchmark_hmat_hodlr_quick.py [--dims 1,2] [--csv out.csv]
"""
import sys

import benchmark_hmat_hodlr as bench

# Reduced target sizes: exercises all algorithms and backends at a small scale
sys.argv = [sys.argv[0], "--targets", "50,100,200"] + [
    arg for arg in sys.argv[1:] if not arg.startswith("--targets")
]

if __name__ == "__main__":
    bench.main()
