"""
Benchmark: LAPACK vs HMAT vs HODLR for GaussianProcess and Kriging.

Tests power-of-2 marginal discretizations across 1D-4D with
IsotropicCovarianceModel (MaternModel 5/2) at corr=0.1 and 1.0.

Measures: 1st realization, 2nd realization, GaussianProcessFitter, GaussianProcessRegression.

Accuracy mode (--accuracy): for each config with n <= --accuracy-max-n,
assembles the dense reference C and the HODLR matrix C_H and reports the
compression-quality metrics of Litvinenko et al. 2019:

  rel_norm2 : ||C - C_H||_2 / ||C||_2          (power iteration on C - C_H)
  gap2      : ||C C_H^-1 - I||_2               (SVD of X - I, X = C C_H^-1)
  kl        : KL(N(0,C) || N(0,C_H)) = 0.5 * (log|C_H| - log|C| + tr(C C_H^-1) - n)
  logdet    : log|C_H| - log|C|

Usage:
  python3 benchmark_gp_full.py                  # timing sweep
  python3 benchmark_gp_full.py --accuracy       # accuracy sweep
"""
import openturns as ot
import time
import sys
import argparse

import numpy as np

ot.Log.Show(ot.Log.NONE)

DIMENSIONS = [1, 2, 3, 4]
CORRELATION_LENGTHS = [0.1, 1.0]
LAPACK_LIMIT = 2 ** 14  # 16384 max total points
HODLR_LIMIT = 2 ** 18  # 262144 max total points
METHODS = ["LAPACK", "HMAT", "HODLR"]
HMAT_FACTOR = "LLt"
HODLR_FACTOR = "LLt"
ACCURACY_DENSE_LIMIT = 2000


def format_time(t):
    if t is None:
        return "N/A"
    if t < 0.001:
        return f"{t * 1e6:.0f}us"
    elif t < 1.0:
        return f"{t * 1e3:.0f}ms"
    elif t < 60.0:
        return f"{t:.2f}s"
    else:
        return f"{t / 60:.1f}m"


def cell(r, key):
    if r and r.get(key) is not None:
        return format_time(r[key])
    return "  ---  "


def make_trend_function(dim):
    terms = []
    for i in range(dim):
        if i % 2 == 0:
            terms.append(f"sin(x{i})")
        else:
            terms.append(f"cos(x{i})")
    return ot.SymbolicFunction([f"x{i}" for i in range(dim)], [" + ".join(terms)])


def run_benchmark(dim, corr, method, n_intervals):
    n_total = (n_intervals + 1) ** dim

    interval = ot.Interval([-1.0] * dim, [1.0] * dim)
    mesher = ot.IntervalMesher([n_intervals] * dim)
    mesh = mesher.build(interval)
    vertices = mesh.getVertices()

    matern = ot.MaternModel([corr], [1.0], 2.5)
    model = ot.IsotropicCovarianceModel(matern, 1)

    # ---- GP realizations ----
    t_first = None
    t_second = None
    try:
        gp = ot.GaussianProcess(model, mesh)
        if method == "LAPACK":
            gp.setSamplingMethod(ot.GaussianProcess.CHOLESKY)
        elif method == "HMAT":
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

    # ---- GaussianProcessFitter ----
    f = make_trend_function(dim)
    X = vertices
    max_train = min(500, n_total)
    step = max(1, n_total // max_train)
    X_train = X[::step]
    Y = f(X_train)
    basis = ot.ConstantBasisFactory(dim).build()

    t_fitter = None
    t_regression = None
    try:
        ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", method)

        fitter = ot.GaussianProcessFitter(X_train, Y, model, basis)
        fitter.setOptimizeParameters(False)

        t0 = time.time()
        fitter.run()
        t_fitter = time.time() - t0

        # ---- GaussianProcessRegression ----
        regression = ot.GaussianProcessRegression(fitter.getResult())

        t0 = time.time()
        regression.run()
        t_regression = time.time() - t0
    except Exception:
        pass

    return {"dim": dim, "corr": corr, "method": method,
            "n_intervals": n_intervals, "n_total": n_total,
            "t_first": t_first, "t_second": t_second,
            "t_fitter": t_fitter, "t_regression": t_regression}


def accuracy_metrics(model, vertices, eps):
    """Compression-quality metrics of the HODLR matrix vs the dense reference.

    C is the dense LAPACK covariance, C_H the HODLR (LLt) approximation.
    Returns a dict with the Litvinenko et al. 2019 metrics:
      rel_norm2 : ||C - C_H||_2 / ||C||_2   (power iteration on the symmetric C - C_H)
      gap2      : ||C C_H^-1 - I||_2        (largest singular value of X - I)
      kl        : KL(N(0,C) || N(0,C_H))    (trace + logdet formula)
      logdet    : log|C_H| - log|C|
    """
    n = vertices.getSize()

    C = np.asarray(model.discretize(vertices))
    chol = np.linalg.cholesky(C)
    logdet_C = 2.0 * np.sum(np.log(np.diag(chol)))
    lam_max_C = float(np.max(np.linalg.eigvalsh(C)))

    p = ot.HODLRMatrixParameters()
    p.setAssemblyEpsilon(eps)
    p.setRecompressionEpsilon(eps)
    p.setMinLeafSize(16)
    hodlr = model.discretizeHODLRMatrix(vertices, p)
    hodlr.factorize()
    logdet_H = hodlr.logDeterminant()

    # X = C C_H^-1: solve C_H x_j = c_j for each column of C.
    X = np.empty((n, n))
    for j in range(n):
        X[:, j] = np.asarray(hodlr.solve(ot.Point(C[:, j])))
    trace_X = float(np.trace(X))
    kl = 0.5 * (trace_X - n + logdet_H - logdet_C)
    gap2 = float(np.linalg.norm(X - np.eye(n), 2))

    # ||C - C_H||_2 by power iteration on the symmetric difference B = C - C_H.
    def b_apply(v):
        y = ot.Point(n, 0.0)
        hodlr.gemv("N", 1.0, ot.Point(v), 0.0, y)
        return C.dot(v) - np.asarray(y)

    vec = np.random.default_rng(0).normal(size=n)
    vec /= float(np.linalg.norm(vec))
    lam = 0.0
    for _ in range(500):
        w = b_apply(vec)
        lam_new = float(np.dot(vec, w))
        nw = float(np.linalg.norm(w))
        if nw < 1e-30:
            break
        vec = w / nw
        if abs(lam_new - lam) <= 1e-12 * (1.0 + abs(lam_new)):
            lam = lam_new
            break
        lam = lam_new
    rel_norm2 = abs(lam) / lam_max_C

    return {"n": n, "rel_norm2": rel_norm2, "gap2": gap2,
            "kl": kl, "logdet": logdet_H - logdet_C}


def run_accuracy(dim, corr, n_intervals, eps):
    n_total = (n_intervals + 1) ** dim
    interval = ot.Interval([-1.0] * dim, [1.0] * dim)
    mesh = ot.IntervalMesher([n_intervals] * dim).build(interval)
    vertices = mesh.getVertices()

    matern = ot.MaternModel([corr], [1.0], 2.5)
    model = ot.IsotropicCovarianceModel(matern, dim)

    m = accuracy_metrics(model, vertices, eps)
    m["n_total"] = n_total
    return m


def print_accuracy_table(dim, corr, results):
    print(f"\n  corr={corr}")
    header = (f"{'n':>6}  "
              f"{'||C-CH||_2/||C||_2':>20}  "
              f"{'||C CH^-1 - I||_2':>20}  "
              f"{'KL':>14}  "
              f"{'logdet diff':>14}")
    print(header)
    print("-" * len(header))
    for r in results:
        print(f"{r['n']:>6}  "
              f"{r['rel_norm2']:>20.3e}  "
              f"{r['gap2']:>20.3e}  "
              f"{r['kl']:>14.3e}  "
              f"{r['logdet']:>14.3e}")


def main_accuracy(eps, dense_limit):
    for dim in DIMENSIONS:
        print(f"\n{'=' * 100}")
        print(f"  {dim}D  |  HODLR accuracy vs dense LAPACK (eps={eps}, n<={dense_limit})")
        print(f"{'=' * 100}")

        for corr in CORRELATION_LENGTHS:
            results = []
            n_intervals = 2
            while True:
                n_total = (n_intervals + 1) ** dim
                if n_total > dense_limit:
                    break
                try:
                    r = run_accuracy(dim, corr, n_intervals, eps)
                    results.append(r)
                    sys.stdout.write(".")
                    sys.stdout.flush()
                except Exception as e:
                    sys.stdout.write("x")
                    sys.stdout.flush()
                    sys.stderr.write(f"  [{dim}D corr={corr} n={n_total}] "
                                     f"{type(e).__name__}: {e}\n")
                    results.append({"n": n_total, "n_total": n_total,
                                    "rel_norm2": float("nan"), "gap2": float("nan"),
                                    "kl": float("nan"), "logdet": float("nan")})
                n_intervals *= 2
            print_accuracy_table(dim, corr, results)


def print_table(dim, corr, results):
    print(f"\n  corr={corr}")
    header = (
        f"{'n':>5} {'pts':>7}"
        f"  {'LAPACK':>37}"
        f"  {'HMAT':>37}"
        f"  {'HODLR':>37}"
    )
    sub = (
        f"{'':5} {'':7}"
        f"  {'1st':>9} {'2nd':>9} {'fit':>9} {'regr':>9}"
        f"  {'1st':>9} {'2nd':>9} {'fit':>9} {'regr':>9}"
        f"  {'1st':>9} {'2nd':>9} {'fit':>9} {'regr':>9}"
    )
    print(header)
    print(sub)
    print("-" * len(sub))

    by_n = {}
    for r in results:
        by_n.setdefault(r['n_intervals'], {})[r['method']] = r

    for n_intervals in sorted(by_n.keys()):
        n_total = (n_intervals + 1) ** dim
        row = f"{n_intervals:>5} {n_total:>7}"
        for method in METHODS:
            r = by_n[n_intervals].get(method)
            row += "  "
            row += f"{cell(r, 't_first'):>9} "
            row += f"{cell(r, 't_second'):>9} "
            row += f"{cell(r, 't_fitter'):>9} "
            row += f"{cell(r, 't_regression'):>9}"
        print(row)


def main():
    ap = argparse.ArgumentParser(description="LAPACK vs HMAT vs HODLR benchmark")
    ap.add_argument("--accuracy", action="store_true",
                    help="measure HODLR compression accuracy vs the dense LAPACK "
                         "reference instead of timing")
    ap.add_argument("--accuracy-max-n", type=int, default=ACCURACY_DENSE_LIMIT,
                    help="dense size limit for the accuracy mode (default %(default)s)")
    ap.add_argument("--eps", type=float, default=1e-6,
                    help="HODLR assembly/recompression epsilon (default %(default)s)")
    args = ap.parse_args()

    if args.accuracy:
        main_accuracy(args.eps, args.accuracy_max_n)
        return

    all_results = []

    for dim in DIMENSIONS:
        print(f"\n{'=' * 120}")
        print(f"  {dim}D  |  MaternModel(nu=2.5, scale=corr), IsotropicCovarianceModel")
        print(f"{'=' * 120}")

        for corr in CORRELATION_LENGTHS:
            results = []
            n_intervals = 2
            while True:
                n_total = (n_intervals + 1) ** dim
                if n_total > HODLR_LIMIT:
                    break
                methods = METHODS if n_total <= LAPACK_LIMIT else ["HMAT", "HODLR"]
                for method in methods:
                    try:
                        r = run_benchmark(dim, corr, method, n_intervals)
                        results.append(r)
                        all_results.append(r)
                        sys.stdout.write(".")
                        sys.stdout.flush()
                    except Exception:
                        sys.stdout.write("x")
                        sys.stdout.flush()
                        results.append({"dim": dim, "corr": corr, "method": method,
                                        "n_intervals": n_intervals, "n_total": n_total,
                                        "t_first": None, "t_second": None,
                                        "t_fitter": None, "t_regression": None})
                n_intervals *= 2
            print_table(dim, corr, results)

    print(f"\n\n{'=' * 120}")
    print("  DONE")
    print(f"{'=' * 120}")


if __name__ == "__main__":
    main()
