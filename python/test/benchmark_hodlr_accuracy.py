"""Accuracy reference comparison of the covariance-matrix backends against a
dense LAPACK Cholesky reference.

Backends (auto-detected on the running build):
  - HODLR      : self-contained HODLRMatrix implementation (branch)
  - HMAT_HODLR : hmat-oss HODLR compression (PR #3272 build)
  - HMAT       : hmat-oss standard admissibility (both builds, context)

Metrics (Litvinenko et al. 2019):
  - rel_solve  : || C_H^-1 b - C^-1 b ||_2 / || C^-1 b ||_2
  - rel_norm2  : || C - C_H ||_2 / || C ||_2            (power iteration)
  - gap2       : || C C_H^-1 - I ||_2                   (largest singular value)
  - logdet_diff: log|C_H| - log|C|
  - kl         : KL(N(0,C) || N(0,C_H)) = 0.5 (tr(C C_H^-1) - n + logdet_diff)

C is the dense LAPACK reference (model.discretize), C_H the HMatrix/HODLR
approximation probed through its gemv (assembly quality, before factorization)
and its solve/log-determinant (end-to-end quality, after factorization).

The hmat-oss factorize() of PR #3272 always shifts the matrix by
2*maxEV*HMatrix-RegularizationEpsilon before factorization (reported through
regularization_shift); the branch HODLR only regularizes a failing
factorization.  Both behaviors are measured as-is.

Run:
  python3 benchmark_hodlr_accuracy.py --csv out.csv
"""
import argparse
import time

import numpy as np
import openturns as ot

ot.Log.Show(ot.Log.NONE)

DIMENSIONS = [1, 2]
INTERVALS = {1: [200, 500, 1000, 2000], 2: [16, 24, 32]}
COVARIANCE_MODELS = ["matern", "exponential"]
CORRELATION_LENGTHS = [0.1, 0.01]
ASSEMBLY_EPSILON = 1.0e-6
MAX_N_FULL_METRICS = 1100
NP_ITERS = 200


def make_model(cov, dim, corr):
    if cov == "matern":
        matern = ot.MaternModel([corr], [1.0], 2.5)
        return ot.IsotropicCovarianceModel(matern, dim)
    return ot.ExponentialModel([corr] * dim, [1.0])


def gemv_wrapper(H, n):
    ybuf = ot.Point(n)

    def f(v):
        H.gemv("N", 1.0, ot.Point(v), 0.0, ybuf)
        return np.asarray(ybuf)
    return f


def dominant_sym_eigen(numpyC, H, n, rng):
    """Largest magnitude eigenvalue of C - C_H via power iteration."""
    hgemv = gemv_wrapper(H, n)
    v = rng.normal(size=n)
    v /= np.linalg.norm(v)
    for _ in range(NP_ITERS):
        Bv = numpyC.dot(v) - hgemv(v)
        nrm = np.linalg.norm(Bv)
        if nrm < 1e-300:
            break
        v = Bv / nrm
    return abs(float(v.dot(numpyC.dot(v) - hgemv(v))))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", default=None, help="CSV output file")
    args = parser.parse_args()

    has_self_hodlr = hasattr(ot.CovarianceModel, "discretizeHODLRMatrix")
    has_hmat_hodlr = hasattr(ot.HMatrixParameters, "setAdmissibility")
    methods = []
    if has_self_hodlr:
        methods.append("HODLR")
    if has_hmat_hodlr:
        methods.append("HMAT_HODLR")
    methods.append("HMAT")
    print(f"build supports: {methods}")

    hmat_params = ot.HMatrixParameters()
    if hasattr(hmat_params, "setAssemblyEpsilon"):
        hmat_params.setAssemblyEpsilon(ASSEMBLY_EPSILON)
        hmat_params.setRecompressionEpsilon(ASSEMBLY_EPSILON)

    rows = []
    rng = np.random.default_rng(1234)

    for dim in DIMENSIONS:
        for ncell in INTERVALS[dim]:
            mesh = ot.IntervalMesher([ncell] * dim).build(ot.Interval([-1.0] * dim, [1.0] * dim))
            vertices = mesh.getVertices()
            n = vertices.getSize()
            for cov in COVARIANCE_MODELS:
                for corr in CORRELATION_LENGTHS:
                    model = make_model(cov, dim, corr)
                    numpyC = np.asarray(model.discretize(vertices))
                    eigenC = np.linalg.eigvalsh(numpyC)
                    eigenC = eigenC[eigenC > 1e-12]
                    logdetC = float(np.sum(np.log(eigenC)))

                    b = rng.normal(size=n)
                    xd = np.linalg.solve(numpyC, b)
                    maxEV = float(np.max(np.abs(eigenC)))
                    eps_reg = ot.ResourceMap.GetAsScalar("HMatrix-RegularizationEpsilon")
                    lambda_fallback = 2.0 * maxEV * eps_reg

                    print(f"[dim={dim} cov={cov} corr={corr} n={n}] "
                          f"cond={np.linalg.cond(numpyC):.2e} minEV={eigenC[0]:.2e}", flush=True)

                    for method in methods:
                        full = n <= MAX_N_FULL_METRICS
                        row = {"dim": dim, "cov": cov, "corr": corr, "n": n, "method": method}
                        shift = "N/A"
                        try:
                            if method == "HODLR":
                                params = ot.HODLRMatrixParameters()
                                params.setMinLeafSize(64)
                                params.setAssemblyEpsilon(ASSEMBLY_EPSILON)
                                params.setRecompressionEpsilon(ASSEMBLY_EPSILON)
                                H = model.discretizeHODLRMatrix(vertices, params)
                                # HODLRMatrix.gemv requires factorization and
                                # then applies A (factor L L^T reconstruction).
                                t0 = time.time()
                                H.factorize()
                                row["t_factorize"] = f"{time.time() - t0:.4f}"
                                spec_B = dominant_sym_eigen(numpyC, H, n, rng)
                                row["rel_norm2"] = f"{spec_B / float(np.max(np.abs(eigenC))):.6e}"
                            elif method == "HMAT_HODLR":
                                params = ot.HMatrixParameters(hmat_params)
                                params.setAdmissibility("hodlr")
                                params.setFactorizationMethod("hodlrsym")
                                H = model.discretizeHMatrix(vertices, params)
                                # hmat-oss gemv applies A before factorization.
                                spec_B = dominant_sym_eigen(numpyC, H, n, rng)
                                row["rel_norm2"] = f"{spec_B / float(np.max(np.abs(eigenC))):.6e}"
                                t0 = time.time()
                                H.factorize("hodlrsym")
                                row["t_factorize"] = f"{time.time() - t0:.4f}"
                            else:
                                params = ot.HMatrixParameters(hmat_params)
                                params.setFactorizationMethod("LLt")
                                H = model.discretizeHMatrix(vertices, params)
                                spec_B = dominant_sym_eigen(numpyC, H, n, rng)
                                row["rel_norm2"] = f"{spec_B / float(np.max(np.abs(eigenC))):.6e}"
                                t0 = time.time()
                                H.factorize("LLt")
                                row["t_factorize"] = f"{time.time() - t0:.4f}"

                            if hasattr(H, "getRegularizationShift"):
                                shift = f"{float(H.getRegularizationShift()):.3e}"
                            row["regularization_shift"] = shift
                            # Applied regularization shift (bounded as
                            # 2*maxEV*eps for builds without the getter).
                            shift0 = float(H.getRegularizationShift()) if hasattr(H, "getRegularizationShift") else lambda_fallback
                            cr = np.asarray(H.compressionRatio(), dtype=float).ravel()
                            row["compression"] = f"{float(cr[0]):.4f}" if cr.size else "N/A"
                            row["lambda"] = f"{shift0:.3e}"

                            # Neutralized reference: C + shift*I (same operator
                            # the backend factorized), to isolate compression
                            # accuracy from the regularization policy.
                            numpyCref = numpyC + shift0 * np.eye(n)
                            eigref = eigenC + shift0
                            logdetCref = float(np.sum(np.log(eigref)))
                            xdref = np.linalg.solve(numpyCref, b)

                            logdetH = float("nan")
                            try:
                                logdet = H.logDeterminant if hasattr(H, "logDeterminant") else H.computeLogDeterminant
                                logdetH = float(logdet())
                            except Exception as e:
                                print(f"    [{method}] logdet unavailable: {type(e).__name__}: {e}")
                            row["logdet_diff"] = f"{logdetH - logdetC:.6f}"
                            row["logdet_diff_ref"] = f"{logdetH - logdetCref:.6f}"

                            xh = np.asarray(H.solve(ot.Point(b)))
                            row["rel_solve"] = f"{np.linalg.norm(xh - xd) / np.linalg.norm(xd):.6e}"
                            row["rel_solve_ref"] = f"{np.linalg.norm(xh - xdref) / np.linalg.norm(xdref):.6e}"

                            if full:
                                X = np.empty((n, n))
                                for j in range(n):
                                    X[:, j] = np.asarray(H.solve(ot.Point(numpyC[:, j])))
                                gap2 = float(np.linalg.svd(X - np.eye(n), compute_uv=False)[0])
                                row["gap2"] = f"{gap2:.6e}"
                                row["kl"] = f"{0.5 * (np.trace(X) - n + logdetH - logdetC):.6e}"
                                row["kl_ref"] = f"{0.5 * (np.trace(X) - n + logdetH - logdetCref):.6e}"
                            else:
                                row["gap2"] = "N/A"
                                row["kl"] = "N/A"
                                row["kl_ref"] = "N/A"
                        except Exception as e:
                            print(f"    [{method}] FAILED: {type(e).__name__}: {e}")
                            for key in ("rel_norm2", "t_factorize", "regularization_shift",
                                        "compression", "logdet_diff", "rel_solve", "gap2", "kl",
                                        "lambda", "logdet_diff_ref", "rel_solve_ref", "kl_ref"):
                                row[key] = "N/A"
                        rows.append(row)
                        print(f"    [{method}] rel_solve={row['rel_solve']} (ref {row.get('rel_solve_ref', 'N/A')}) "
                              f"rel_norm2={row.get('rel_norm2', 'N/A')} "
                              f"gap2={row.get('gap2', 'N/A')} kl={row.get('kl', 'N/A')} "
                              f"logdet_diff={row['logdet_diff']} t_fact={row['t_factorize']}s "
                              f"lambda={row.get('lambda', 'N/A')}", flush=True)

    header = ["dim", "cov", "corr", "n", "method", "rel_solve", "rel_solve_ref", "rel_norm2",
              "gap2", "logdet_diff", "logdet_diff_ref", "kl", "kl_ref", "t_factorize",
              "compression", "regularization_shift", "lambda"]
    if args.csv:
        with open(args.csv, "w") as f:
            f.write(",".join(header) + "\n")
            for r in rows:
                f.write(",".join(str(r[k]) for k in header) + "\n")
        print(f"wrote {args.csv}: {len(rows)} rows")
    else:
        print("\t".join(header))
        for r in rows:
            print("\t".join(str(r[k]) for k in header))


if __name__ == "__main__":
    main()
