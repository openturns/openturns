"""
Benchmark of the covariance-matrix backends available for GP-related algorithms.

Algorithms covered:
  - GaussianProcess.getRealization() (1st call factorizes, 2nd call reuses)
  - KarhunenLoeveP1Algorithm with the SPECTRA eigen-solver (compressed storage)
  - GaussianProcessFitter
  - GaussianProcessRegression

Methods compared (per-algorithm ResourceMap backend):
  - LAPACK  : dense Cholesky / dense eigen solve
  - HMAT    : hmat-oss assemb+factorization with the standard admissibility rule
  - HMAT_HODLR : hmat-oss HODLR compression (admissibility 'hodlr'),
                 requires PR #3272 to be merged (falls back to N/A otherwise)
  - HODLR   : self-contained HODLRMatrix implementation

Sweep: dims 1..4, Matern(5/2) and ExponentialModel covariance models, correlation
length 0.1 and 0.01 (relative to the [-1,1]^dim domain), and matrix sizes about
1000, 2000, 5000, 10000, 20000 (actual sizes depend on IntervalMesher).

Run:
  python3 benchmark_hmat_hodlr.py
  python3 benchmark_hmat_hodlr.py --dims 1,2 --max-n 5000 --csv out.csv
"""
import argparse
import os
import sys
import time

import openturns as ot

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.NONE)

DIMENSIONS = [1, 2, 3, 4]
CORRELATION_LENGTHS = [0.1, 0.01]
TARGET_SIZES = [1000, 2000, 5000, 10000, 20000]
METHODS = ["LAPACK", "HMAT", "HMAT_HODLR", "HODLR"]
COVARIANCE_MODELS = ["matern", "exponential"]
KL_NB_MODES = 20
LAPACK_GP_LIMIT = 5000    # dense Cholesky still affordable below this size
LAPACK_KL_LIMIT = 4096    # dense eigen solve scales as (dim * n)^3


def format_time(t):
    if t is None:
        return "N/A"
    if t < 0.001:
        return f"{t * 1e6:.0f}us"
    if t < 1.0:
        return f"{t * 1e3:.1f}ms"
    if t < 60.0:
        return f"{t:.2f}s"
    return f"{t / 60.0:.1f}m"


def intervals_for_target(dim, target):
    """Number of intervals per axis so that the mesh has ~target vertices."""
    return max(1, round(target ** (1.0 / dim)))


def make_model(cov, dim, corr):
    """Matern(5/2) or Exponential covariance model with scale corr."""
    if cov == "matern":
        matern = ot.MaternModel([corr], [1.0], 2.5)
        return ot.IsotropicCovarianceModel(matern, dim)
    return ot.ExponentialModel([corr] * dim, [1.0])


def make_trend_function(dim):
    terms = []
    for i in range(dim):
        if i % 2 == 0:
            terms.append(f"sin(x{i})")
        else:
            terms.append(f"cos(x{i})")
    return ot.SymbolicFunction([f"x{i}" for i in range(dim)], [" + ".join(terms)])


def set_optional_string(key, value):
    """Set a ResourceMap string key, creating it if not registered yet."""
    if ot.ResourceMap.HasKey(key):
        ot.ResourceMap.SetAsString(key, value)
    else:
        ot.ResourceMap.AddAsString(key, value)


_warned_setup = set()


def setup_backend(method):
    """Configure every backend touched by the benchmarked algorithms."""
    gp_la = {
        "LAPACK": "LAPACK",
        "HMAT": "HMAT",
        "HMAT_HODLR": "HMAT",
        "HODLR": "HODLR",
    }[method]
    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", gp_la)

    if method == "LAPACK":
        kl_storage = "DENSE"
        kl_solver = "LAPACK"
    else:
        kl_storage = gp_la  # HMAT or HODLR
        kl_solver = "SPECTRA"
    ot.ResourceMap.SetAsString("KarhunenLoeveP1Algorithm-CovarianceMatrixStorage", kl_storage)
    ot.ResourceMap.SetAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver", kl_solver)

    # hmat-oss settings (HMatrix-Admissibility appears with PR #3272).
    # HODLR compression is driven by the admissibility rule: the assembly
    # uses hmat_create_admissibility_hodlr() when the key is "hodlr".
    factorization = "hodlr" if method == "HMAT_HODLR" else "LLt"
    try:
        ot.ResourceMap.SetAsString("HMatrix-FactorizationMethod", factorization)
    except Exception:
        if method not in _warned_setup:
            _warned_setup.add(method)
            sys.stderr.write(f"  [note] {method}: the ResourceMap key "
                             "'HMatrix-FactorizationMethod' does not accept '{factorization}' "
                             "(enum limited to LU/LDLt/LLt); using LLt. HODLR compression is "
                             "enabled through the HMatrix-Admissibility key.\n")
        ot.ResourceMap.SetAsString("HMatrix-FactorizationMethod", "LLt")
    admissibility = "hodlr" if method == "HMAT_HODLR" else "standard"
    set_optional_string("HMatrix-Admissibility", admissibility)


def benchmark_sampling(method, mesh, model):
    """Time GaussianProcess.getRealization(): first and second call."""
    t_first = None
    t_second = None
    try:
        gp = ot.GaussianProcess(model, mesh)
        if method == "LAPACK":
            gp.setSamplingMethod(ot.GaussianProcess.CHOLESKY)
        elif method in ("HMAT", "HMAT_HODLR"):
            gp.setSamplingMethod(ot.GaussianProcess.HMAT)
        else:
            gp.setSamplingMethod(ot.GaussianProcess.HODLR)

        t0 = time.time()
        gp.getRealization()
        t_first = time.time() - t0

        t0 = time.time()
        gp.getRealization()
        t_second = time.time() - t0
    except Exception:
        pass
    return t_first, t_second


def benchmark_kl(mesh, model, nb_modes):
    """Time KarhunenLoeveP1Algorithm.run() with a fixed number of modes."""
    t = None
    try:
        kl = ot.KarhunenLoeveP1Algorithm(mesh, model, 0.5)
        kl.setNbModes(nb_modes)
        t0 = time.time()
        kl.run()
        t = time.time() - t0
    except Exception:
        pass
    return t


def benchmark_fit(X_train, Y, model, basis):
    """Time GaussianProcessFitter.run() and GaussianProcessRegression.run()."""
    t_fitter = None
    t_regression = None
    try:
        fitter = ot.GaussianProcessFitter(X_train, Y, model, basis)
        fitter.setOptimizeParameters(False)

        t0 = time.time()
        fitter.run()
        t_fitter = time.time() - t0

        regression = ot.GaussianProcessRegression(fitter.getResult())
        t0 = time.time()
        regression.run()
        t_regression = time.time() - t0
    except Exception:
        pass
    return t_fitter, t_regression


def run_row(dim, corr, cov, method, intervals, nb_modes):
    """Benchmark one (method, size) combination on a dim-dimensional mesh."""
    n_total = (intervals + 1) ** dim

    interval = ot.Interval([-1.0] * dim, [1.0] * dim)
    mesh = ot.IntervalMesher([intervals] * dim).build(interval)

    setup_backend(method)

    row = {
        "dim": dim,
        "corr": corr,
        "cov": cov,
        "method": method,
        "n_total": n_total,
        "t_first": None,
        "t_second": None,
        "t_kl": None,
        "t_fitter": None,
        "t_regression": None,
    }

    model = make_model(cov, dim, corr)

    # GaussianProcess sampling (first and second realization)
    if method != "LAPACK" or n_total <= LAPACK_GP_LIMIT:
        row["t_first"], row["t_second"] = benchmark_sampling(method, mesh, model)

    # KarhunenLoeveP1Algorithm (dense eigen solve only for small augmented matrices)
    if method != "LAPACK" or dim * n_total <= LAPACK_KL_LIMIT:
        row["t_kl"] = benchmark_kl(mesh, model, nb_modes)

    # GaussianProcessFitter + GaussianProcessRegression on a fixed-size training set
    X = mesh.getVertices()
    max_train = min(500, n_total)
    step = max(1, n_total // max_train)
    X_train = X[::step]
    f = make_trend_function(dim)
    Y = f(X_train)
    basis = ot.ConstantBasisFactory(dim).build()
    row["t_fitter"], row["t_regression"] = benchmark_fit(X_train, Y, model, basis)

    return row


def print_table(results):
    jobs = [("1st", "t_first"), ("2nd", "t_second"), ("KL", "t_kl"),
            ("fit", "t_fitter"), ("regr", "t_regression")]
    header = f"{'n':>7} {'pts':>7}"
    sub = f"{'':7} {'':7}"
    for method in METHODS:
        header += f"  {method:>44}"
        sub += "  " + " ".join(f"{label:>8}" for label, _ in jobs)
    print(header)
    print(sub)
    print("-" * len(sub))

    by_n = {}
    for r in results:
        by_n.setdefault(r["n_total"], {})[r["method"]] = r

    for n_total in sorted(by_n.keys()):
        row = f"{n_total:>7} {n_total:>7}"
        for method in METHODS:
            r = by_n[n_total].get(method)
            cells = "  " + " ".join(f"{format_time(None if r is None else r[key]):>8}"
                                    for _, key in jobs)
            row += cells
        print(row)


def main():
    ap = argparse.ArgumentParser(
        description="LAPACK vs HMAT vs HMAT_HODLR vs HODLR benchmark for GP algorithms")
    ap.add_argument("--dims", type=str, default=",".join(map(str, DIMENSIONS)),
                    help="space-separated dims, e.g. 1,2,3,4")
    ap.add_argument("--corr", type=str, default=",".join(map(str, CORRELATION_LENGTHS)),
                    help="correlation lengths, e.g. 0.1,0.01")
    ap.add_argument("--cov", type=str, default=",".join(COVARIANCE_MODELS),
                    help="covariance models, e.g. matern,exponential")
    ap.add_argument("--methods", type=str, default=",".join(METHODS),
                    help="backends, e.g. LAPACK,HMAT,HMAT_HODLR,HODLR")
    ap.add_argument("--max-n", type=int, default=TARGET_SIZES[-1],
                    help="largest target matrix size (default %(default)s)")
    ap.add_argument("--targets", type=str, default=None,
                    help="override the target sizes entirely, e.g. 50,100,200")
    ap.add_argument("--nb-modes", type=int, default=KL_NB_MODES,
                    help="number of KL modes requested (default %(default)s)")
    ap.add_argument("--csv", type=str, default=None,
                    help="append results to a CSV file")
    args = ap.parse_args()

    dims = [int(x) for x in args.dims.split(",")]
    corrs = [float(x) for x in args.corr.split(",")]
    covs = args.cov.split(",")
    methods = args.methods.split(",")
    if args.targets is None:
        targets = [n for n in TARGET_SIZES if n <= args.max_n]
    else:
        targets = [int(x) for x in args.targets.split(",")]

    if args.csv and not os.path.exists(args.csv):
        with open(args.csv, "w", encoding="utf-8") as csv_file:
            csv_file.write("dim,cov,corr,method,n_total,"
                           "t_first,t_second,t_kl,t_fitter,t_regression\n")

    for cov in covs:
        print(f"\n{'=' * 110}")
        print(f"  Covariance model: {cov}")
        print(f"{'=' * 110}")
        for dim in dims:
            print(f"\n  {dim}D  |  corr in {corrs}, targets in {targets}")
            print(f"  {'.' * 100}")
            results = []
            for corr in corrs:
                for target in targets:
                    intervals = intervals_for_target(dim, target)
                    n_total = (intervals + 1) ** dim
                    for method in methods:
                        try:
                            r = run_row(dim, corr, cov, method, intervals, args.nb_modes)
                            results.append(r)
                            sys.stdout.write(".")
                            sys.stdout.flush()
                            if args.csv:
                                with open(args.csv, "a", encoding="utf-8") as csv_file:
                                    csv_file.write(
                                        ",".join(str(r[k]) for k in
                                                 ("dim", "cov", "corr", "method",
                                                  "n_total", "t_first", "t_second",
                                                  "t_kl", "t_fitter", "t_regression"))
                                        + "\n")
                        except Exception as exc:
                            sys.stdout.write("x")
                            sys.stdout.flush()
                            sys.stderr.write(f"  [err {dim}D {corr} {method} n~{n_total}] "
                                             f"{type(exc).__name__}: {exc}\n")
                            r = {"dim": dim, "corr": corr, "cov": cov, "method": method,
                                 "n_total": n_total, "t_first": None, "t_second": None,
                                 "t_kl": None, "t_fitter": None, "t_regression": None}
                            results.append(r)
                            if args.csv:
                                with open(args.csv, "a", encoding="utf-8") as csv_file:
                                    csv_file.write(
                                        ",".join(str(r[k]) for k in
                                                 ("dim", "cov", "corr", "method",
                                                  "n_total", "t_first", "t_second",
                                                  "t_kl", "t_fitter", "t_regression"))
                                        + "\n")
                    print(f"  (n~{target})", end=" ", flush=True)
                print()
                print_table([r for r in results if r["corr"] == corr])
    print("\nDONE")


if __name__ == "__main__":
    main()
