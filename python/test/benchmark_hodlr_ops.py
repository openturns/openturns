"""Pure-comparison benchmark of the three core covariance-matrix linear-algebra
operations against a dense LAPACK reference.

Operations benchmarked per (dim, cov, corr, n) configuration:
  - Cholesky decomposition       (dense np.linalg.cholesky vs HODLR/HMAT factorize)
  - matrix/vector product        (dense matmul vs backend gemv, probe based)
  - log determinant              (dense Cholesky diag vs backend logDeterminant)

Backends (auto-detected on the running build):
  - LAPACK      : dense NumPy reference (OpenBLAS)
  - HODLR       : self-contained HODLRMatrix implementation (branch build)
  - HMAT_HODLR  : hmat-oss HODLR compression (PR #3272 build, hodlrsym)
  - HMAT        : hmat-oss standard admissibility (both builds, context)

Sweep (default): dims 1..4, Matern(5/2) and Exponential, correlation lengths
0.1 and 0.01, sizes up to n=40001 in 1D and n up to just under the dense
(OpenBLAS) affordability limit in higher dimensions (~20000-29000).

Nothing is fixed/healed here: every backend is measured as-is and both the
always-on regularization shift of the PR#3272 hmat-oss factorize() and the
error-triggered pivot-floor shift of the self-contained HODLR are recorded.

Run (from the repo root, per-invocation PYTHONPATH as usual):
  python3 python/test/benchmark_hodlr_ops.py --csv results_ops.csv
  python3 python/test/benchmark_hodlr_ops.py --smoke   # 2 tiny configs
"""
import argparse
import time

import numpy as np
import openturns as ot

ot.Log.Show(ot.Log.NONE)

DIMS = [1, 2, 3, 4]
CORR = [0.1, 0.01]
COVS = ["matern", "exponential"]
ASSEMBLY_EPSILON = 1.0e-6
# (intervals+1)^dim target sizes, chosen so the largest stays within the dense
# LAPACK (OpenBLAS) affordability band on this machine.
TARGETS = {
    1: [1000, 2000, 5000, 10000, 20000, 30000],
    2: [32, 44, 70, 99, 141],
    3: [10, 13, 16, 22, 27],
    4: [6, 7, 9, 10, 12],
}
NP_MATVEC_PROBES = 4
COND_EIG_LIMIT = 5001  # eigvalsh (O(n^3)) only below this size

HEADER = ["dim", "cov", "corr", "n", "intervals", "npts", "method",
          "t_assembly", "t_factorize", "t_solve", "t_matvec", "t_matvec_post",
          "rel_solve", "rel_matvec", "rel_matvec_pre", "rel_matvec_post",
          "logdet_diff", "logdet_rel", "t_logdet",
          "compression", "regularization_shift"]


def csv_write_row(path, row):
    """Append one row to the CSV, creating the header when the file is new."""
    with open(path, "a") as f:
        if f.tell() == 0:
            f.write(",".join(HEADER) + "\n")
        f.write(",".join(str(row.get(k, "")) for k in HEADER) + "\n")


def make_model(cov, dim, corr):
    if cov == "matern":
        matern = ot.MaternModel([corr], [1.0], 2.5)
        return ot.IsotropicCovarianceModel(matern, dim)
    return ot.ExponentialModel([corr] * dim, [1.0])


def wrap_gemv(H, n):
    ybuf = ot.Point(n)

    def f(v):
        H.gemv("N", 1.0, ot.Point(v), 0.0, ybuf)
        return np.asarray(ybuf)
    return f


def probe_matvec(hgemv, yd, V, n):
    """Probe-based matvec error (max over probes) and median probe time."""
    best = np.inf
    times = []
    for k in range(V.shape[1]):
        t0 = time.time()
        yh = hgemv(V[:, k])
        times.append(time.time() - t0)
        nrm = float(np.linalg.norm(yd[:, k]))
        if nrm > 0.0:
            best = min(best, float(np.linalg.norm(yh - yd[:, k])) / nrm)
    err = best if best < np.inf else float("nan")
    tmed = float(np.median(times[1:])) if len(times) > 1 else float(times[0])
    return f"{err:.6e}", f"{tmed:.4e}"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", default=None)
    ap.add_argument("--smoke", action="store_true",
                    help="run a 2-config subset to validate the harness")
    ap.add_argument("--no-hmat", action="store_true",
                    help="skip the standard-admissibility HMAT backend")
    ap.add_argument("--hmat-max", type=int, default=None,
                    help="skip standard HMAT rows above this n (it behaves like "
                         "dense; keeps the run inside the time budget)")
    ap.add_argument("--max-target", type=int, default=None,
                    help="skip targets above this value (cheap truncation)")
    ap.add_argument("--max-minutes", type=float, default=360,
                    help="hard wall-clock deadline for the sweep (default 360)")
    args = ap.parse_args()

    t_start = time.time()

    has_self_hodlr = hasattr(ot.CovarianceModel, "discretizeHODLRMatrix")
    has_hmat_hodlr = hasattr(ot.HMatrixParameters, "setAdmissibility")
    methods = ["LAPACK"]
    if has_self_hodlr:
        methods.append("HODLR")
    if has_hmat_hodlr and not args.no_hmat:
        methods.append("HMAT_HODLR")
    if not args.no_hmat:
        methods.append("HMAT")
    print(f"build supports: {methods}", flush=True)

    hmat_params = ot.HMatrixParameters()
    if hasattr(hmat_params, "setAssemblyEpsilon"):
        hmat_params.setAssemblyEpsilon(ASSEMBLY_EPSILON)
        hmat_params.setRecompressionEpsilon(ASSEMBLY_EPSILON)

    rows = []
    rng = np.random.default_rng(2026)

    targets = {d: TARGETS[d] for d in DIMS}
    if args.max_target:
        for d in DIMS:
            targets[d] = [t for t in targets[d] if t <= args.max_target]

    for dim in DIMS:
        if time.time() - t_start > args.max_minutes * 60:
            print(f"deadline reached, stopping at dim={dim}", flush=True)
            break
        for t in targets[dim]:
            intervals = t if dim == 1 else t
            mesh = ot.IntervalMesher([intervals] * dim).build(
                ot.Interval([-1.0] * dim, [1.0] * dim))
            vertices = mesh.getVertices()
            n = vertices.getSize()
            npts = n
            for cov in COVS:
                for corr in CORR:
                    if args.smoke and not (dim == 1 and t == targets[1][0]
                                           and cov == "matern" and corr == 0.1):
                        continue
                    model = make_model(cov, dim, corr)
                    t0 = time.time()
                    numpyC = np.asarray(model.discretize(vertices))
                    t_dense_asm = time.time() - t0

                    t0 = time.time()
                    Ld = np.linalg.cholesky(numpyC)
                    t_dense_fact = time.time() - t0
                    logdetC = float(2.0 * np.sum(np.log(np.diag(Ld))))

                    cond = "N/A"
                    if n <= COND_EIG_LIMIT:
                        ev = np.linalg.eigvalsh(numpyC)
                        cond = f"{ev[-1] / ev[0]:.3e}"

                    b = rng.normal(size=n)
                    t0 = time.time()
                    xd = np.linalg.solve(numpyC, b)
                    t_dense_solve = time.time() - t0

                    V = rng.normal(size=(n, NP_MATVEC_PROBES))
                    t_probes = []
                    yd = np.zeros((n, NP_MATVEC_PROBES))
                    for k in range(NP_MATVEC_PROBES):
                        t0 = time.time()
                        yd[:, k] = numpyC.dot(V[:, k])
                        t_probes.append(time.time() - t0)
                    t_dense_matvec = float(np.median(t_probes))

                    print(f"[dim={dim} cov={cov} corr={corr} n={n}] "
                          f"cond={cond} dense_fact={t_dense_fact:.3f}s "
                          f"logdet={logdetC:.6f}", flush=True)

                    for method in methods:
                        if method == "HMAT" and args.hmat_max and n > args.hmat_max:
                            continue
                        row = {"dim": dim, "cov": cov, "corr": corr, "n": n,
                               "intervals": intervals, "npts": npts, "method": method,
                               "t_assembly": "N/A", "t_factorize": "N/A",
                               "t_solve": "N/A", "t_matvec": "N/A",
                               "t_matvec_post": "N/A", "rel_solve": "N/A",
                               "rel_matvec": "N/A", "rel_matvec_pre": "N/A",
                               "rel_matvec_post": "N/A",
                               "logdet_diff": "N/A", "logdet_rel": "N/A",
                               "t_logdet": "N/A", "compression": "N/A",
                               "regularization_shift": "N/A"}
                        if method == "LAPACK":
                            row["t_assembly"] = f"{t_dense_asm:.4f}"
                            row["t_factorize"] = f"{t_dense_fact:.4f}"
                            row["t_solve"] = f"{t_dense_solve:.4f}"
                            row["t_matvec"] = f"{t_dense_matvec:.4f}"
                            row["t_matvec_post"] = f"{t_dense_matvec:.4f}"
                            row["rel_solve"] = "0.0"
                            row["rel_matvec"] = "0.0"
                            row["rel_matvec_pre"] = "0.0"
                            row["rel_matvec_post"] = "0.0"
                            row["logdet_diff"] = "0.0"
                            row["logdet_rel"] = "0.0"
                            rows.append(row)
                            if args.csv:
                                csv_write_row(args.csv, row)
                            continue
                        try:
                            shift = "N/A"
                            if method == "HODLR":
                                params = ot.HODLRMatrixParameters()
                                params.setMinLeafSize(64)
                                params.setAssemblyEpsilon(ASSEMBLY_EPSILON)
                                params.setRecompressionEpsilon(ASSEMBLY_EPSILON)
                                t0 = time.time()
                                H = model.discretizeHODLRMatrix(vertices, params)
                                row["t_assembly"] = f"{time.time() - t0:.4f}"
                                t0 = time.time()
                                H.factorize()
                                row["t_factorize"] = f"{time.time() - t0:.4f}"
                                # HODLR gemv applies A from the factor (LL^T),
                                # so it requires factorization: operational matvec.
                                hgemv = wrap_gemv(H, n)
                                row["rel_matvec_post"], row["t_matvec"] = probe_matvec(
                                    hgemv, yd, V, n)
                                row["rel_matvec"] = row["rel_matvec_post"]
                            elif method == "HMAT_HODLR":
                                params = ot.HMatrixParameters(hmat_params)
                                params.setAdmissibility("hodlr")
                                params.setFactorizationMethod("hodlrsym")
                                t0 = time.time()
                                H = model.discretizeHMatrix(vertices, params)
                                row["t_assembly"] = f"{time.time() - t0:.4f}"
                                # hmat-oss gemv applies A pre-factorization.
                                hgemv_pre = wrap_gemv(H, n)
                                row["rel_matvec_pre"], row["t_matvec"] = probe_matvec(
                                    hgemv_pre, yd, V, n)
                                row["rel_matvec"] = row["rel_matvec_pre"]
                                t0 = time.time()
                                H.factorize("hodlrsym")
                                row["t_factorize"] = f"{time.time() - t0:.4f}"
                            else:
                                params = ot.HMatrixParameters(hmat_params)
                                params.setFactorizationMethod("LLt")
                                t0 = time.time()
                                H = model.discretizeHMatrix(vertices, params)
                                row["t_assembly"] = f"{time.time() - t0:.4f}"
                                hgemv_pre = wrap_gemv(H, n)
                                row["rel_matvec_pre"], row["t_matvec"] = probe_matvec(
                                    hgemv_pre, yd, V, n)
                                row["rel_matvec"] = row["rel_matvec_pre"]
                                t0 = time.time()
                                H.factorize("LLt")
                                row["t_factorize"] = f"{time.time() - t0:.4f}"
                        except Exception as e:
                            print(f"    [{method}] FAILED: {type(e).__name__}: {e}", flush=True)
                            for key in ("t_assembly", "t_factorize", "t_solve", "t_matvec",
                                        "t_matvec_post", "rel_solve", "rel_matvec",
                                        "rel_matvec_pre", "rel_matvec_post",
                                        "logdet_diff", "logdet_rel", "t_logdet",
                                        "compression", "regularization_shift"):
                                row[key] = "N/A"
                            rows.append(row)
                            if args.csv:
                                csv_write_row(args.csv, row)
                            continue
                        try:

                            if hasattr(H, "getRegularizationShift"):
                                shift = f"{float(H.getRegularizationShift()):.3e}"
                            row["regularization_shift"] = shift
                            cr = np.asarray(H.compressionRatio(), dtype=float).ravel()
                            row["compression"] = f"{float(cr[0]):.4f}" if cr.size else "N/A"

                            # solve vs the dense reference
                            t0 = time.time()
                            xh = np.asarray(H.solve(ot.Point(b)))
                            row["t_solve"] = f"{time.time() - t0:.4f}"
                            row["rel_solve"] = f"{np.linalg.norm(xh - xd) / np.linalg.norm(xd):.6e}"

                            # post-factorization gemv, for reference (HODLR already
                            # recorded it as the operational matvec above)
                            if method != "HODLR":
                                hgemv_post = wrap_gemv(H, n)
                                row["rel_matvec_post"], row["t_matvec_post"] = probe_matvec(
                                    hgemv_post, yd, V, n)

                            # log determinant
                            func = None
                            if hasattr(H, "logDeterminant"):
                                func = H.logDeterminant
                            elif hasattr(H, "computeLogDeterminant"):
                                func = H.computeLogDeterminant
                            if func is not None:
                                t0 = time.time()
                                logdetH = float(func())
                                row["t_logdet"] = f"{time.time() - t0:.4f}"
                                d = logdetH - logdetC
                                row["logdet_diff"] = f"{d:.6e}"
                                row["logdet_rel"] = f"{abs(d / logdetC):.6e}"
                            else:
                                print(f"    [{method}] no log-determinant API", flush=True)
                        except Exception as e:
                            print(f"    [{method}] FAILED: {type(e).__name__}: {e}", flush=True)
                            for key in ("t_assembly", "t_factorize", "t_solve", "t_matvec",
                                        "rel_solve", "rel_matvec", "logdet_diff",
                                        "logdet_rel", "t_logdet", "compression",
                                        "regularization_shift"):
                                row[key] = "N/A"
                        rows.append(row)
                        if args.csv:
                            csv_write_row(args.csv, row)
                        print(f"    [{method}] t_fact={row['t_factorize']}s "
                              f"rel_solve={row['rel_solve']} rel_matvec={row['rel_matvec']} "
                              f"logdet_diff={row['logdet_diff']} shift={row['regularization_shift']}",
                              flush=True)

    if args.csv:
        print(f"\nwrote {args.csv}: {len(rows)} rows total")
    else:
        print("\t".join(HEADER))
        for r in rows:
            print("\t".join(str(r[k]) for k in HEADER))


if __name__ == "__main__":
    main()
