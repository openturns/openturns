# Sparse GP Analytic Gradients - Session State
_Last updated: 2026-08-19_

## Objective
Implement the action plan in `doc/SparseGP/analytic_gradient_and_evolution.tex`.
The report contains the full derivations and was written in a prior session.

## Task context
The user asked: "can you translate the recommendations into an action plan,
then start the implementation?" then interrupted with "stop the session as
soon as possible. Save the state for a future session and tell me how to
restart the work."

## STATUS: ACTION PLAN STEP 1 IS DONE (2026-08-18)
Analytic `parameterGradient(s,t)` implemented + verified for 12 covariance
models in `lib/src/Base/Stat/` (was: finite-difference fallback at
`CovarianceModelImplementation.cxx:291`). All verified against central FD
in `/tmp/opencode/verify_cov_param_grad.py` (all `[OK]`, max rel diff
~1e-9 for analytic, ~5e-8 for the FD-fallback Kronecker case). Library
builds clean; existing tests pass
(`ctest -R "pyinstallcheck_(CovarianceModel|ExponentiallyDampedCosineModel|ExponentialModel|ProductCovarianceModel|StationaryFunctionalCovarianceModel|GaussianProcessFitter|SparseGaussianProcessFitter)_std"`).

## STATUS: ACTION PLAN STEP 2 IS DONE (2026-08-18)
Analytic ELBO gradient for `SparseGaussianProcessFitter` implemented and
verified, replacing the `NonCenteredFiniteDifferenceGradient` in
`getObjectiveFunction()`. Details:
- New `computeELBOGradient(parameters)` (SparseGaussianProcessFitter.cxx)
  does the unpacking (as computeELBO), the forward sweep (as
  computeELBOValue), then the reverse sweep and the assembly
  (covariance params via `parameterGradient`, inducing points via
  `partialGradient`, noise via `2 sigma^2 sigma2bar`). Output Point layout
  = optimization parameter layout.
- New nested `ELBOGradient : GradientImplementation` class
  (SparseGaussianProcessFitter.hxx) wired via
  `objective.setGradient(ELBOGradient(*this).clone())`.
- New static `cholAdjoint(L, LBar)` helper (Murray's adjoint algorithm).
- Verified vs central FD on all parameter blocks and M<N / M==N:
  max abs diff ~1.5e-8 (`/tmp/opencode/verify_elbo_grad.py`, ALL PASS).
- Cantilever-beam example now reaches R2=0.9929 WITHOUT input
  standardization (target was R2>0.98).
- Bugs found during verification: (a) the report/session derivations were
  MISSING the `Abar += yperp w^T/sigma^2` term (from the direct
  A-dependence of yperp = y - Aw); (b) first C++ cholAdjoint draft divided
  `Lbar_jj` by `L_jj` an extra time (line `Lbar_jj -= Nbar_ij L_ij` is
  correct); (c) `-(N-M)` on UnsignedInteger wraps to 2^64 -> compute the
  signed difference with `SignedInteger(N) - SignedInteger(M)`.
- Regression test added: `test_elbo_gradient_all_parameters` in
  python/test/t_SparseGaussianProcessFitter_std.py.
- NOTE: covariance model `setActiveParameter([0,1])` makes
  `getParameter()` report amplitude = 1e-12 regardless of setAmplitude
  (pre-existing OT quirk); use `setParameter([scale, amp])` in tests.

## STATUS: ACTION PLAN STEP 3 IS DONE (2026-08-19)
Analytic reduced log-likelihood gradient for `GaussianProcessFitter`,
replacing the `NonCenteredFiniteDifferenceGradient` in
`getReducedLogLikelihoodFunction()` (was at GaussianProcessFitter.cxx:683-684).
Details:
- New `computeReducedLogLikelihoodGradient(parameters)` (GaussianProcessFitter.cxx)
  does the forward sweep exactly as computeLapackLogDeterminantCholesky (C via
  discretize + noise, L = C.computeRegularizedCholesky, rho0 = L^{-1}y, optional
  basis Phi = L^{-1}F, beta least squares, rho = rho0 - Phi beta), then the
  reverse sweep (LBar_logDet diag 2/L_ii; LBar_q via rhoBar = w*2*rho with
  w = N/s if analyticalAmplitude_ else 1; basis adjoint: PhiBar, betaBar,
  z = S^{-1}betaBar, SBar, LBar += -L^{-T}PhiBar Phi^T and -L^{-T}rho0Bar rho0^T),
  CB = cholAdjoint(L, LBar), then assembly over the lower triangle of C:
  gradient[k] = -0.5 * CB(gi,gj) * parameterGradient(xi,xj)(k, b*p+a), with
  gi = i*p+a, gj = j*p+b, col = b*p+a (multivariate column layout).
- New nested `ReducedLogLikelihoodGradient : GradientImplementation` class
  (GaussianProcessFitter.hxx) wired via
  `logLikelihood.setGradient(ReducedLogLikelihoodGradient(*this).clone())`.
- Reuses the static `cholAdjoint(L, LBar)` helper (duplicated from
  SparseGaussianProcessFitter.cxx). HMAT method throws NotYetImplemented
  (LAPACK-only).
- Verified vs central FD (`/tmp/opencode/verify_gp_grad.py`, ALL PASS):
  1D non-analytical (no basis / basis / noise), 1D analytical amplitude
  (no basis / basis), 2D multivariate tensorized (no basis / basis),
  max abs err ~3e-6 in well-conditioned settings. Numpy reference:
  `/tmp/opencode/gp_grad_ref.py` (all 4 cases ~2.5e-9).
- Regression test added: `test_reduced_loglikelihood_gradient` in
  python/test/t_GaussianProcessFitter_std.py (tests both the general and the
  analytical-amplitude modes).
- NOTE on conditioning: near-singular C (scale >> data range), the FD of the
  regularized value function diverges from the analytic gradient of the
  unregularized likelihood (computeRegularizedCholesky jitter depends on C).
  Use well-conditioned reference points in tests. The gradient at scale=1.0 on
  range-8 data is meaningless either way.
- NOTE on the fitter lifetime: `ReducedLogLikelihoodEvaluation/Gradient` hold a
  reference to the fitter; do NOT chain
  `ot.GaussianProcessFitter(...).getReducedLogLikelihoodFunction()` in tests
  (temporary fitter destroyed -> dangling reference -> segfault). Keep the
  fitter alive.

## STATUS: ACTION PLAN STEP 4 IS DONE (2026-08-19)
Analytic metamodel Hessian for `SparseGaussianProcessRegression`, replacing
the `CenteredFiniteDifferenceHessian` at `SparseGaussianProcessRegression.cxx:87`.
Details:
- New `CovarianceModelImplementation::partialHessian(s,t)` virtual returning
  a `SymmetricMatrix` (Hessian of the FIRST output component wrt s) with a
  base 4-point central-FD fallback of the FUNCTION (NOT the gradient — FD of
  the forward-FD gradient amplifies noise to ~1e-3), `h = pow(ScalarEpsilon,0.25)`.
  Public wrapper on `CovarianceModel`, SWIG docstrings
  (`OT_CovarianceModel_partialHessian_doc`).
- Analytic `partialHessian` added to all 12 models that have analytic
  `parameterGradient`/`partialGradient` (commits listed below), each verified
  vs 4-point function-FD. Key formulas (all radial `k=a^2 rho(z)`,
  `u_i=tau_i/theta_i^2`, `H_ab = factor*u_a*u_b + delta_ab*diagFactor/theta_a^2`):
  - SquaredExponential: `factor=k` SE, `diagFactor=-k`, `z=0.5||tau/theta||^2`.
  - MaternModel: `factor=a^2 e((2nu-2)K'+(z^2+2nu^2-2nu)K/z)/z^3`,
    `diagFactor=a^2 e(nu K/z + K')/z`, `e=exp(logNorm+nu log z)` (radial
    correction REQUIRED; `z==0` -> FD fallback).
  - AbsoluteExponential (separable L1): `H_ab = k sign(tau_a)sign(tau_b)/(theta_a theta_b)`,
    sign convention `(tau>0 -> -1)`.
  - GeneralizedExponential: `factor=a^2 p z^(p-4)(p z^p - p + 2) e^-z^p`,
    `diagFactor=-a^2 p z^(p-2) e^-z^p`.
  - ExponentialModel (multivariate, output (0,0)): GE p=1 scaled by `outputCovariance_(0,0)`.
  - ExponentiallyDampedCosineModel: `factor=a^2 e^-z(cos(1+z-w^2 z)+w sin(1+2z))/z^3`,
    `diagFactor=-a^2 e^-z(cos + w sin)/z`, `w=2 pi frequency`.
  - SphericalModel: `factor=a^2 1.5(1+z^2)/z^3`, `diagFactor=a^2 1.5(z^2-1)/z`,
    WITH `u_i=tau_i/(theta_i^2 R^2)` and `diag/(theta_a^2 R^2)` (R = radius;
    the R factors are REQUIRED, first draft missed them). Outside support
    (z>=1) -> zero Hessian.
  - IsotropicCovarianceModel: radial rule with `K'`/`K''` from the 1D kernel
    via `partialGradient`/`partialHessian` at `(tauNorm, 0)`.
  - FractionalBrownianMotionModel: `H_11 = 0.5 a^2 2H(2H-1)/theta^2 (|s/theta|^(2H-2) - |(t-s)/theta|^(2H-2))`
    (singular at s=0 or s=t -> FD fallback).
  - DiracCovarianceModel: zero Hessian.
  - ProductCovarianceModel: within-block `(total/k_i)*Hess_i`, cross-block
    `(total/(k_i k_j))*grad_i(a) grad_j(b)`, once per unordered pair (first
    draft double-counted the cross terms by adding both (i,j) and (j,i)).
  - StationaryFunctionalCovarianceModel: `H_ab = a^2 HessRho(u)_ab sign_a sign_b/(theta_a theta_b)`,
    `u_i=|tau_i|/theta_i` (also FIXED a pre-existing bug: `partialGradient` was
    missing the `1/theta_i` factor).
- New `SparseGaussianProcessHessian : HessianImplementation` class
  (`result(j,k,0) = sum_m alpha[m] * partialHessian(x,Z_m)(j,k)`, same
  `alpha = whiteningFactor_.transpose().solveLinearSystem(posteriorMean_)`
  machinery as the gradient), wired in `SparseGaussianProcessRegression::run`.
- Verified metamodel Hessian vs `CenteredFiniteDifferenceHessian(1e-4, eval)`
  on a 3D example: max abs diff ~1e-6. Regression test added:
  `test_hessian` in python/test/t_SparseGaussianProcessRegression_std.py.
  New class registered: CMakeLists (lib+python), experimental_module.i,
  `SparseGaussianProcessHessian.{i,_doc.i}`, user_manual/surrogate_modeling.rst,
  ChangeLog.

## NEXT MOVE: none pending in the action plan
Action plan step 4 (last) is complete. Optional follow-ups: analytic
`partialGradient`/`partialHessian` for Kronecker/Tensorized/RankM covariance
models (still FD), or removing the now-unused `CenteredFiniteDifferenceHessian`
include dependency.

### Models with analytic parameterGradient
1. SquaredExponential (univariate)
2. MaternModel (univariate; nu gradient via FD on a clone, rest analytic)
3. AbsoluteExponential (univariate)
4. GeneralizedExponential (univariate; includes p)
5. ExponentialModel (univariate + full multivariate incl. R_ij)
6. ExponentiallyDampedCosineModel (univariate; includes frequency)
7. SphericalModel (univariate; includes radius)
8. IsotropicCovarianceModel (delegates to kernel at tauNorm)
9. FractionalBrownianMotionModel (univariate; includes H)
10. DiracCovarianceModel (univariate + multivariate)
11. ProductCovarianceModel (product rule over marginals)
12. StationaryFunctionalCovarianceModel (scale + rho params)

### Mechanism
- New protected helpers in CovarianceModelImplementation:
  `filterActiveParameterGradient(const Point&)` and
  `filterActiveParameterGradient(const Matrix&)` — build the FULL gradient
  wrt the full parameter layout then keep rows `activeParameter_`.
  Overrides call `computeAsScalar(s,t)` / `operator()(s,t)` etc.
- Return shape: `Matrix(nActive, 1)` univariate, `Matrix(nActive, p*p)`
  multivariate (column `j*outputDim+i` for element (i,j)), matching FD fallback.
- Models still on FD fallback (not implemented): KroneckerCovarianceModel,
  TensorizedCovarianceModel, RankMCovarianceModel, UserDefinedCovarianceModel,
  UserDefinedStationaryCovarianceModel. CauchyModel is spectral (not a
  covariance model); Compact/Full/Null are filtering windows.

## ACTION PLAN (from report Sec. 7)
2. Analytic ELBO gradient for SparseGaussianProcessFitter (report Sec. 3) - DONE (2026-08-18).
3. Analytic log-likelihood gradient for GaussianProcessFitter (follow-up):
   Replace FD at `GaussianProcessFitter.cxx:683-684`; M=N limit of ELBO sweep - DONE (2026-08-19).
4. Analytic metamodel Hessian for SparseGaussianProcessRegression (follow-up):
   Replace `CenteredFiniteDifferenceHessian` at
   `SparseGaussianProcessRegression.cxx:87` - DONE (2026-08-19).
5. Do NOT touch KrigingAlgorithm / GeneralLinearModelAlgorithm (deprecated).

## EXACT DERIVATIVES (already derived + numerically verified - use directly)
Verified reference: `/tmp/opencode/verify_final.py` (matches central FD:
scales 2e-9, amp 2e-8, log-sigma 5e-9, inducing points 4e-6).

### Forward (as coded in computeELBOValue, `SparseGaussianProcessFitter.cxx:435-520`)
- Luu=chol(Kuu); A=Kfu.Luu^{-T} (coded: LuuInvKuf=Luu.solve(Kuf), A=LuuInvKuf^T)
- G=AtA (CovarianceMatrix); B=G+sigma^2 I; Lb=chol(B)
- Aty=A^T y; w=G^{-1}Aty (via Lg=chol(G)); u=Lb^{-1}w; yperp=y-Aw
- ELBO = -0.5(N log2pi + (N-M) log sigma^2 + logdet B + q) - traceTerm
  with q=||w||^2 - sigma^2 ||u||^2 + ||yperp||^2/sigma^2
  and traceTerm=(trKff-||A||_F^2)/(2 sigma^2), only if M<N.

### Reverse sweep (all barX = dL/dX)
- ybar_perp = -yperp/sigma^2
- ubar = sigma^2 u
- wbar = -w + A^T yperp/sigma^2 + sigma^2 Lb^{-T} u
- Lbbar = -sigma^2 (Lb^{-T}u) u^T ; diag += -1/Lb_ii
- tbar = G^{-1} wbar (solve via Lg); Gbar = -(G^{-1}wbar) w^T
- Abar += y tbar^T
- Abar += yperp w^T/sigma^2      <-- IMPORTANT: direct A-dependence of yperp=y-Aw
- Bbar = cholAdjoint(Lb, Lbbar); Gbar += Bbar
- dL/dsigma^2 += tr(Bbar)
- Abar += A(Gbar + Gbar^T)
- if M<N: Abar += A/sigma^2 ; Kffbar = -I/(2 sigma^2) (diagonal)
- Kfubar = Abar Luu^{-1}
- Luubar = -Luu^{-T}(Abar^T Kfu)Luu^{-T}
- Kuubar = cholAdjoint(Luu, Luubar)

### Cholesky adjoint (Murray 2016), lower L, A=L L^T
```
for j=n-1..0:
  for i=j+1..n-1:
    Nbar_ij = Lbar_ij/L_jj; Lbar_jj -= Lbar_ij L_ij/L_jj; Abar_ij += Nbar_ij
    for k<j: Lbar_ik -= Nbar_ij L_jk; Lbar_jk -= Nbar_ij L_ik
  sbar_j = Lbar_jj/(2 L_jj); Abar_jj += sbar_j
  for k<j: Lbar_jk -= 2 sbar_j L_jk
```
Note: computeRegularizedCholesky adds fixed jitter -> no gradient through it.

### Noise (optimization variable is log sigma, sigma=std, noiseVariance=std)
- dL/dsigma^2 = tr(Bbar) + 0.5||u||^2 + ||yperp||^2/(2 sigma^4)
  + (trKff-||A||_F^2)/(2 sigma^4) - (N-M)/(2 sigma^2)
- dL/d(log sigma) = 2 sigma^2 * dL/dsigma^2

### Kernel derivatives (SquaredExponential k=a^2 exp(-0.5 sum (xd-zd)^2/ld^2))
- dk/d a = 2k/a ; dk/d ld = k (xd-zd)^2/ld^3 ; dk/d zd = k (xd-zd)/ld^2
- Assembly: dL/dtheta = sum_ij Kfubar_ij dk(xi,zj)/dtheta
  + sum_pq Kuubar_pq dk(zp,zq)/dtheta + sum_i Kffbar_ii dk(xi,xi)/dtheta
- Inducing points:
  dL/dz_jd = sum_i Kfubar_ij k(xi,zj)(xi_d-zj_d)/ld^2
    + sum_q Kuubar_jq k(zj,zq)(zq_d-zj_d)/ld^2
    + sum_p Kuubar_pj k(zp,zj)(zp_d-zj_d)/ld^2

## KEY CODE FACTS FOR IMPLEMENTATION
- `getParameter()` returns ACTIVE params only (ordered by activeParameter_).
  Full layout (getFullParameterDescription, CovarianceModelImplementation.cxx:995):
  [scale_0..scale_{D-1}, nuggetFactor, amplitude_0.., R_ij...]; kernel-specific
  appended (Matern: "nu" at index D+2).
- Default activeParameter_ (CovarianceModelImplementation.cxx:59-60): all indices
  [0, D+1+outDim) EXCEPT index D (nugget). So Matern nu is NOT active by default.
- setParameter/getParameter: CovarianceModelImplementation.cxx:1082-1099
- parameterGradient FD fallback: CovarianceModelImplementation.cxx:291-335
- Spatial partialGradient formula for SE: SquaredExponential.cxx:105-121
- MaternModel: scale/amplitude/nu layout, setFullParameter/getFullParameter at
  MaternModel.cxx:177-220; partialGradient at MaternModel.cxx:137-167
- ELBO code: SparseGaussianProcessFitter.cxx:435-520 (forward),
  computeELBO:395-432 (unpack), buildOptimizationParameters:544-558,
  maximizeELBO:290-393, getObjectiveFunction:143-151
- TriangularMatrix API: solveLinearSystem(Point/Matrix), transpose();
  Matrix API: transpose, solveLinearSystem, operator*; Point: normSquare,
  operator*. TriangularMatrix lower-triangular by default.
- Function::setGradient(const Gradient&) at lib/src/Base/Func/openturns/Function.hxx:99
- SpecFunc has LogBesselK/BesselK/BesselKDerivative(nu,x) (d/dx only, no d/dnu)
- Default solver TNC, OptimizationNormalization=true
  (lib/src/Base/Common/ResourceMap.cxx:1639,1644)

## HOW TO RESTART
1. Steps 1-4 are DONE (2026-08-18/19): analytic covariance-model
   parameterGradient (12 models, committed as e1d25e6b6), analytic ELBO
   gradient for SparseGaussianProcessFitter (committed as 75293c73b + fix
   c9f0ae3e8, see /tmp/opencode/verify_elbo_grad.py), analytic reduced
   log-likelihood gradient for GaussianProcessFitter (committed as 667485659,
   see /tmp/opencode/verify_gp_grad.py and gp_grad_ref.py), and analytic
   metamodel Hessian for SparseGaussianProcessRegression (committed as
   series of partialHessian commits below).
2. The action plan is complete. Any follow-up work would be extending the
   analytic partialGradient/partialHessian to Kronecker/Tensorized/RankM
   covariance models (still FD). See "STATUS: ACTION PLAN STEP 4 IS DONE".
3. Build: `cmake --build build --target install --parallel $(( nproc / 2 ))`
   (from /home/regis/openturns-dev). For Python, use
   `PYTHONPATH=/home/regis/openturns-dev/build/install/lib64/python3.13/site-packages`
   (NOT ~/.local which is a stale July install without sparse classes).
4. Verify: `ctest -R "pyinstallcheck_(CovarianceModel|SparseGaussianProcessRegression|SparseGaussianProcessFitter|GaussianProcessFitter)_std"`.