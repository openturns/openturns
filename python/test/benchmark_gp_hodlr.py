"""
Benchmark: LAPACK vs HMAT vs HODLR for GaussianProcess fitting.

Tests power-of-2 sizes:
  - n^dim < 2^16 (65536): all three (LAPACK, HMAT, HODLR)
  - 2^16 <= n^dim < 2^20 (1048576): HMAT + HODLR only

Uses ExponentialModel on [-1,1]^dim, IntervalMesher.
"""
import openturns as ot
import time

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.NONE)
DIMENSIONS = [1, 2, 3, 4]
CORRELATION_LENGTHS = [0.1, 1.0]
MAX_TOTAL = 2**16  # 1048576
LAPACK_LIMIT = 2**12  # 65536


def format_time(t):
    if t is None:
        return "N/A"
    if t < 0.001:
        return f"{t * 1e6:.0f}us"
    elif t < 1.0:
        return f"{t * 1e3:.1f}ms"
    elif t < 60.0:
        return f"{t:.2f}s"
    else:
        m = int(t // 60)
        s = t - m * 60
        return f"{m}m{s:.0f}s"


def make_trend_function(dim):
    terms = []
    for i in range(dim):
        if i % 2 == 0:
            terms.append(f"sin(x{i})")
        else:
            terms.append(f"cos(x{i})")
    return ot.SymbolicFunction([f"x{i}" for i in range(dim)], [" + ".join(terms)])


def run_benchmark(dim, corr, method, n):
    n_total = n ** dim

    interval = ot.Interval([-1.0] * dim, [1.0] * dim)
    mesher = ot.IntervalMesher([n] * dim)
    mesh = mesher.build(interval)

    model = ot.ExponentialModel([corr] * dim, [1.0])

    X = mesh.getVertices()
    max_train = min(500, n_total)
    step = max(1, n_total // max_train)
    X_train = X[::step]

    f = make_trend_function(dim)
    Y = f(X_train)
    basis = ot.ConstantBasisFactory(dim).build()

    ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", method)

    fitter = ot.GaussianProcessFitter(X_train, Y, model, basis)
    fitter.setOptimizeParameters(False)

    t0 = time.time()
    fitter.run()
    t_fitter = time.time() - t0

    regression = ot.GaussianProcessRegression(fitter.getResult())

    t0 = time.time()
    regression.run()
    t_regression = time.time() - t0

    t_first = None
    t_second = None
    if method in ("LAPACK", "HMAT", "HODLR"):
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

    return {
        "dim": dim,
        "corr": corr,
        "method": method,
        "n": n,
        "n_total": n_total,
        "t_fitter": t_fitter,
        "t_regression": t_regression,
        "t_first": t_first,
        "t_second": t_second,
    }


def cell(r, key):
    return format_time(r[key]) if r and r.get(key) is not None else " --- "


def print_table(dim, corr, results):
    print(f"\n  corr={corr}")
    header = (
        f"{'n':>6} {'n^dim':>9}"
        f"  {'-- LAPACK --':>37}"
        f"  {'--- HMAT ---':>37}"
        f"  {'-- HODLR ---':>37}"
    )
    sub = (
        f"{'':6} {'':9}"
        f"  {'fitter':>9} {'regress':>9} {'1st_real':>9} {'2nd_real':>9}"
        f"  {'fitter':>9} {'regress':>9} {'1st_real':>9} {'2nd_real':>9}"
        f"  {'fitter':>9} {'regress':>9} {'1st_real':>9} {'2nd_real':>9}"
    )
    print(header)
    print(sub)
    print("-" * len(sub))

    by_n = {}
    for r in results:
        by_n.setdefault(r['n'], {})[r['method']] = r

    for n in sorted(by_n.keys()):
        row = f"{n:>6} {n**dim:>9}"
        for method in ("LAPACK", "HMAT", "HODLR"):
            r = by_n[n].get(method)
            row += "  "
            row += f"{cell(r, 't_fitter'):>9} "
            row += f"{cell(r, 't_regression'):>9} "
            row += f"{cell(r, 't_first'):>9} "
            row += f"{cell(r, 't_second'):>9}"
        print(row)


def main():
    all_results = []

    for dim in DIMENSIONS:
        print(f"\n{'=' * 100}")
        print(f"  {dim}D")
        print(f"{'=' * 100}")

        for corr in CORRELATION_LENGTHS:
            results = []
            n = 2
            while n ** dim < MAX_TOTAL:
                n_total = n ** dim
                methods = ["LAPACK", "HMAT", "HODLR"] if n_total < LAPACK_LIMIT else ["HMAT", "HODLR"]
                for method in methods:
                    try:
                        r = run_benchmark(dim, corr, method, n)
                        results.append(r)
                        all_results.append(r)
                    except Exception:
                        results.append({
                            "dim": dim, "corr": corr, "method": method,
                            "n": n, "n_total": n_total,
                            "t_fitter": None, "t_regression": None,
                            "t_first": None, "t_second": None,
                        })
                n *= 2
            print_table(dim, corr, results)


if __name__ == "__main__":
    main()
