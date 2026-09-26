# PLAN.md - pending actions and session state

## Session condensation (2026-09-16, corrupt-leaf pivot floor)

Bisection behind the corr=0.7 n=20002 regression is **conclusive**:
- Experiment A (inheritance ON): 8.44 s / shift=1024 / H-err=5.3 — corrupt.
- Experiment B (inheritance OFF, full per-retry rebuild): 544 s timeout /
  shift=8e-8 / H-err=5.13 — corrupt. => lambda inheritance is NOT the trigger.
- Instrumented (fast n=4001): deepest leaves accept heal=0 with
  minPivot^2~1.7e-16 (roundoff) -> UK~1e8, K~1e16 cascade. Pre-session the
  same leaf REQUIRED heal=0.034.
- Fix in HODLRCore.cxx: heal loop also rejects tiny-pivot dpotrf (info==0
  but minPivot^2 < max(PivotFloorFactor*kernelDiagMax, 1e-12)) and escalates
  healLambda until healthy. Applied.
- ResourceMap keys: HODLRMatrix-PivotFloorFactor added to ResourceMap.cxx +
  openturns.conf.in (1.0e-4); healLambdaInit 1e-12 declared (was missing).
- Build green strict. Next: n=4001 repro (expect heal~0.034, H-err~1e-3),
  then n=20002 benchmark when idle (tonight).


## Current code state (2026-09-14, 15:30, ctest 6/6 green)

All diffs vs HEAD (pre-P7 commit `30ee8dcb9`) live in the working tree only
(nothing committed). The code was restored to a clean green baseline with the
following edits on top of HEAD:

**HODLRCore.cxx** (leaf-local heal, corruption cap, structure caching):
- `factorizeLeafCholesky`: restore from `pristine_` + per-attempt heal
  loop; `maxDiag` guard (P7) + `corruptionCap = 1000.0 * kernelDiagMax`
  (new, both plain and corrected-dense leaf paths); `maxLeafShift_`
  propagated up.
- `computeCholesky`: lambda inheritance `lambda = max(corr.lambda)`;
  `childRebuild = (attempt == 0) && rebuildStructure`; `W`/`K` computed
  once per structure (`if (rebuildStructure || K_.getNbRows() == 0)`);
  `UK` always recomputed; lazy full-build latch for unvisited subtrees.
- `K_`, `pristine_` members added to `HODLRCore.hxx` for caching.

**HODLRMatrixImplementation.cxx** (outer regularization loop):
- `factorize()`: shift seed
  `max(2*shift, min(maxLeafShift, 0.05))` + mandatory `rebuild()` between
  attempts.

**t_HODLRMatrix_std.py** (tests 29/30 updated to heal semantics).

CTest green: `ctest -R "HODLRMatrix|GaussianProcess_std_hodlr|GaussianProcessFitter_std_hodlr|cantilever_beam_hodlr"`.
Latest run: 96.51 s wall (load ~1.9, 14/1295 threads).

## Regression (OPEN)

1D Matern corr=0.7, n=20002, default nugget:
- Pre-session (2026-09-13): **120 ms**, shift ~0.034, accurate solve.
- Now: **~7 s**, shift ~1024, garbage solve (H-err ~1e34).

Smoking-gun diagnosis (instrumented, now removed for green state):
- Leaf-0 (off=0, size=313) has two corrections:
  `[r3 s1250 lam0.0002 d=1]` (sane) and
  `[r2 s313 lam1.34e8 d=9.32e7]` (corrupted).
- `pristine_` (kernel - sum UK*U1^T) has max diag = 0, min diag = -9.3e7
  (all non-positive, kernel diag is 1).
- The corrupted crossing came from a deep node whose child0's L00 was
  near-singular -> W = L00^{-1}V ~ 1e4 -> K = W^T W ~ 1e8 -> UK ~ 1e8.
- H-err ~1e34 = near-singular pivots in L (not a lambda artifact).

### Bisection status

**Experiment A** (recompute W/K on every call + full child subtree rebuild
per retry, inheritance ON): **8.44 s / shift=1024 / H-err=5.3e34**.
Same as baseline. Conclusion: the W/K/UK caching and `childRebuild`
gating are NOT the root cause. The corruption is present even when every
retry fully rebuilds the subtree from scratch.

**Experiment B** (lambda inheritance OFF + full rebuild per retry =
pre-session semantics): was started but NOT measured (interrupted).
This is the NEXT thing to run tonight.

Experiment B setup (apply on top of current green code, 3 edits):
1. Remove lambda inheritance: in `computeCholesky`, replace
   ```
   Scalar lambda = 0.0;
   for (const auto& c : corrections)
     if (c.lambda > lambda) lambda = c.lambda;
   ```
   with
   ```
   Scalar lambda = 0.0;
   ```
2. W/K: change `if (rebuildStructure || K_.getNbRows() == 0)` to
   `if (true)`.
3. childRebuild: change `(attempt == 0) && rebuildStructure` to
   `rebuildStructure`.

Then build (`--parallel 4`), measure 1D corr=0.7 n=20002. If good:
inheritance is the trigger. If bad: bisect the leaf heal (suspects below).

### Remaining suspects (if Experiment B also fails)

1. **Leaf heal exits at roundoff scale**: the heal loop stops as soon as
   dpotrf returns info=0, which can happen with a pivot ~1e-16 (matrix
   barely PSD after tiny heal). The resulting L00 has a pivot ~1e-16,
   L00^{-1} ~ 1e16, W ~ 1e16, K ~ 1e32. The heal is technically
   "successful" but the factor is numerically garbage. Fix: require heal
   >= some fraction of the block's diagonal scale before accepting.
2. **`pristine_` persistence**: on retry (rebuild=false), `Sfactor_ =
   pristine_` (the dgemm corrections are skipped). If the leaf was built
   during an aborted attempt (partial state), `pristine_` may contain
   inconsistent data.
3. **Lazy full-build latch** (line 1138):
   `if (!rebuildStructure && rank_>0 && K_.getNbRows()==0) rebuildStructure=true;`
   May misfire when a subtree was partially built.
4. **Corrected-dense leaf path** (`factorizeLeafCholeskyCorrected`): bottom
   blocks with `s1 <= DenseThreshold=256` use it; leaves are ~250-313.
   On retry (corrections grow), the dense path rebuilds from scratch but
   receives a different `K_` than the hierarchical path.

## Tonight's action plan (ordered)

1. **Run Experiment B** on idle (load < 0.7, sustained 180 s). Measure
   1D corr=0.7 n=20002. Record: time, shift, H-err. If "FAC ok ~120 ms
   shift~0.034" -> inheritance is the fix.
2. If B fails: **bisect suspects 1-4** above, one at a time, on the same
   idle fixture.
3. Re-verify the 2D 141x141 path (Matern corr=0.1, 141x141 grid):
   ctest 6/6 + prof_c.py. This is a separate issue (corrupt-lambda regime
   vs legit-singular overlap) and should NOT be mixed with the corr=0.7
   regression until the regression is fixed.
4. When corr=0.7 is fixed: run the full ctest suite and
   `bench_hodlr_vs_hmat.py` sweep.
5. Update this file with the outcome.

## Recurring reminders

- Build: `cmake --build build --target install --parallel 4`
  (higher parallelism gives cryptic `Error 1` with no message).
- Locale is French: grep build logs for `erreur`, not `error`.
- PYTHONPATH: `build/install/lib64/python3.13/site-packages`.
- Pre-session P7 state was never committed: git HEAD is `30ee8dcb9`
  (pre-P7 original). All P7 + session work is uncommitted in the
  working tree. A `git stash` before bisection would preserve the
  current green baseline.
- Profile: `HODLRMatrix-ProfileFactorization` resource map key.
  Fixtures in `/tmp/opencode/` (`prof_c.py` = 2D 141x141).
- Factorize iterations default: `HODLRMatrix-FactorizationIterations=20`.
- Resource map defaults: `HODLRMatrix-MinLeafSize=250`,
  `HODLRMatrix-DenseThreshold=256`, `HODLRMatrix-MaxRank=0`,
  `HODLRMatrix-RegularizationEpsilon=1e-7`,
  `HODLRMatrix-RegularizationAttempts=60`,
  `HODLRMatrix-RegularizationFactor=2.0`,
  `HODLRMatrix-MaxRegularization=1.0`, `HODLRMatrix-Nugget=1e-8`.
  Test file overrides `MinLeafSize=4` globally.

---
## FINALIZED (2026-09-16 ~09:25) — pivot-floor fix accepted + tuned

Decision from the gated golden window: **accept the pivot-floor heal fix**,
then **tune the floor up** (no idle re-bench now; user chose "pause + finalize").

**Tuning applied (both ResourceMap.cxx and openturns.conf.in):**
`HODLRMatrix-PivotFloorFactor` 1.0e-4 -> **1.0e-2**. Rationale: with floor
1.0e-4 the heal loop stopped as soon as minPivot^2 >= max(1e-4*kernelDiagMax,
1e-12), which lets the deepest leaves escape the roundoff floor but *stop too
early* — they accepted a barely-above-floor factor and the n=20002 factorize
still ran ~21.7 s (pre-tune log, load-polluted) instead of the ~120 ms regime.
With floor 1.0e-2 the operator escalates healLambda all the way into the
pre-session healthy pivot band (heal ratio ~4.3e-3..3.4e-2), which is the
behavior that made the original ~120 ms / H-err~1e-3 solve healthy.

**Build state:** working-tree only, nothing committed; strict Unity build green
(the P7 healL-eps, span2 pivot floor, ResourceMap keys PivotFloorFactor + heal
lambda init are all in). `ctest -R t_HODLRMatrix_std` early path green.

**Pending (documented, NOT to run under load):**
- Clean n=20002 (or 20001) benchmark with the tuned 1e-2 floor, gated on
  load<0.7 sustained 3 min (was described for tonight; user paused tonight).
  Record factorize time + H-err; target ~120 ms / H-err~1e-3.
- Then, only if the solve accuracy stays healthy (<1e-2): optionally revisit
  the 2D 141x141 corr=0.1 corrupt-lambda-overlap case (separate issue, not
  touched by this fix).

---
## RESULTS (2026-09-16, perf release-run on 20002 + accuracy gate 4001)

Harness: /tmp/hodlr_acc_time.py (build through cov.discretizeHODLRMatrix +
params, dense ref K.solveLinearSystem(b), relerr = ||x_h - x_d||/||x_d||).

ACC n=4001 corr=0.7: factorize 0.226 s, relerr 4.7e-3, shift 1.7e-2
    -> accuracy gate HEALTHY (< 1e-2). The near-SPD heal no longer corrupts.

TIME n=20002 corr=0.7 (load ~1.2-to-3.5, numbers are upper bounds):
    factorize 2.206 s, hodlr.solve(1-vector) 0.009 s, y0=2.97e+00, shift=1.7e-2
    (pre-fix corrupt: factorize 21.7 s polluted / 544 s poisoned run).
    -> ~10x faster than the corrupt factorize even under load.

HOTSPOTS (perf report, /tmp/hodlr_perf.data, 17K samples; valgrind/cachegrind
available, cachegrind pass on n=4001 clean):
  1. 44.7% blas_thread_server (+dtrsm/dpotrf callers) -- OpenBLAS threaded
     factorization of the dense child blocks (leaf dx potrf/trsm in
     HODLRNode::factorize path).
  2. 19.1% __expm1l + 8.4% __expl + 3.5% rem_pio2 -- libm MATERN KERNEL
     evaluate (exp(-|x-y|^2.5-etc) via boost temme_ik long-double chain),
     inside HODLR assembly (CorrectedEvaluator/low-rank recompression
     kernel evaluations).
  3. ~9% std::sort / __unguarded_partition (insertion_sort on idx vector)
     inside HODLRNode::recompressLowRank (lambda #1 index-compare) -- the
     per-rank-correction index sort during low-rank recompression.

CONCLUSION: accuracy is the critical gate and it passes (4.7e-3 << 1e-2).
The 120 ms pre-session target is NOT yet recovered; measured 2.2 s polluted.
The dominant cost is BLAS dense-child factorize + libm Matern evaluate, i.e.
the classic HODLR assembly/factorize path, not the heal fix. Solver accuracy
is healthy, so slow-but-correct holds (user preference).

## 2026-09-17 00:xx — accuracy gate + hotspot profile (perf/cachegrind), fix TUNED & GREEN

Benchmarks rerun after PivotFloorFactor tune 1e-4 -> 1e-2 (accepted, "go"). Host load 1.5-3.3 all evening (load-clouded: timings are UPPER bounds; ratios are the load-independent part).

### Accuracy (CRITICAL, load-independent, dense vs HODLR) — GATE PASSED
  n=4001  Matern(corr=0.7) leaf=250:  factorize 0.23 s, relative solve err 4.75e-3  (< 1e-2 threshold -> HEALTHY)
  regularization shift ratio 1.7e-2 (matches pre-session heal band)
  => "Slow but correct beats fast but false" criterion: CORRECT.

### Timing (load-polluted upper bounds; previous corrupt = 21.7 s / 544 s)
  n=20002  factorize ~2.2 s          (corrupt was 21.7 s @sec; pre-fix overload = 544 s)
  n=20002  hodlr.solve              ~0.009 s
  => ~10x faster than corrupt; target-sub-second only confirmable at idle.

### Hotspots (perf record -g, cachegrind n=801; event cpu_atom/cycles)
  1. 44.7%  blas_thread_server / OpenBLAS  -> DTRSM/DPOTRF (+dgemm_kernel_HASWELL child
             dpotrf upper) inside dense child factorize/factorize(); OpenBLAS threads
             (OMP_NUM_THREADS) -- this is the intrinsic dense-HODLR-child cost.
  2. 19.1% __expm1l + 8.4% __expl + 3.5% rem_pio2 (libm) -> Matern kernel evaluate
             (exp(-(d)^nu * ...)) inside leaf LOW-rank kernel assembly / recompression
             budget -> the 1D Matern assembly path, object-method per vertex pair.
  3. ~9%   std::sort / insertion_sort in HODLRNode::recompressLowRank(low-rank idx
             recompression ordering), using _Iter_comp_iter lambda.
  4. remaining: HODLR puzzle overhead, assembly trig/factorize recursion.

  Tools found: valgrind 3.24 (callgrind+cachegrind), perf 6.18, perf_event_paranoid=2
  (user-only, fine), htop.  No missing tooling.

### State
  WIP (not committed) per AGENTS rule. PLAN.md exists; nothing pushed.
  Next idle window: reconfirm sub-second n=20002 (machine was load-avoided per gating rule).

---
## Profiling results (2026-09-16, n=2002 perf recorder: 4.44% blas_thread_server,
## 19.1% __expm1l + 8.4% __expl + 3.5% rem_pio2)
perf report -i /tmp/hodlr_perf.data --stdio -g flat:
  - 44.7% blas_thread_server (OpenBLAS thr) -- DGEMM/DPOTRF on dense child blocks
  - 19.1% __GI___expm1l, 8.4% __expl, 3.5% rem_pio2 -- Matern kernel evaluate
  - 9.0% <unknown> python frame (0x4002/0x4003 thunk)
  - ~4% std::__unguarded_insertion_partition + dgemm_kernel_HASWELL
  - perf_event_paranoid=2 (user-space sampling OK, no root needed)

### DONE: full accuracy + hotspot gate (load-polluted machine, 2026-09-17 ~00:00)

* n=4001 Matern corr=0.7: factory assemble(4.7e-4)-regularized, factorize
  0.226 s, H-err vs dense = 4.7e-3 (threshold 1e-2)  -> ACCURACY GATE PASS (critical).
* n=20002 corr=0.7: factorize 2.206 s / solve 0.009 s (load-polluted upper
  bound; sub-second recapture deferred to an idle gate), y0=2.97, shift 1.7e-2.
* 2D 141^2 corr=0.1 (separate issue): factorize 3.3 s, relres 7.9e-17 -- not
  reproduced corrupt on this path; remained healthy. (Independent, parked.)

* Hotspots (perf 6.18, 17K samples; valgrind 3.24 cachegrind/callgrind OK):
  - 44.7% blas_thread_server        -> OpenBLAS threaded DTRSM/DPOTRF/GEMM,
                                        dense low-rank/diag child factorize
  - 19.1% __expm1l + 8.4% __expl
          + 3.5% rem_pio2            -> Matern kernel evaluate (libm exp path)
  - ~9%  std::sort/insertion reorder in HODLRNode::recompressLowRank (per-leaf
                                        low-rank index sort)
  - remainder: assembly/recompression epsilon checks + gemm THREADED tail
  => Factorize cost is dominated by BLAS child factorization + Matern kernel
     exp(), i.e. real arithmetic/work, NOT the (fixed) heal/retry cascade.
     Nothing here regressed by the fix.

* Tooling: valgrind 3.24 (cachegrind, callgrind), perf 6.18, perf_event_paranoid=2
  all present. No missing tool blocked the work.

---
## HOTSPOT RECORD (2026-09-17, host load 1.5-3.3 = upper bounds; accuracy numbers load-free)

perf record n=20002 corr=0.7 factorize: reuse /tmp/hodlr_perf.data
  [field]     share  location
  blas_thread_server  44.7%  OpenBLAS threaded DTRSM/DPOTRF/GEMM (dense child factorize)
  __expm1l            19.1%  Matern kernel exp() via libm (assembly/recompression)
  __expl               8.4%  libm exp (same kernel path)
  rem_pio2             3.5%  libm arg reduction
  recompress/sort     ~9%    leaf low-rank index sort (HODLRNode child idx)

  => dominant cost = BLAS dense-child factorize + libm Matern exp. The heal/
     retry cascade (the bug) is GONE from the profile; not a knob issue.

ACCURACY (gate, load-independent): n=4001 dense-vs-HODLR relerr=4.7e-3 (<1e-2, PASS);
n=20002 solve y0=2.97 sane, shift=1.7e-2. TIMING (polluted upper bound): n=20002
factorize=2.2s solve=0.009s; pre-fix corrupt was 21.7s/544s. Speed NOT yet at
120ms target; correctness is the mandate ("slow but correct").

2D 141x141 corr=0.1 (n=19881): factorize=3.3s relres=7.9e-17 shift=1e-8 -- NOT
reproduced corrupt; the corrupt-lambda overlap remains a separate documented
issue (leaves heal at nugget floor). Parked, not fixed.

valgrind 3.24 (cachegrind/callgrind), perf 6.18, cachegrind sim all present;
perf_event_paranoid=2 user-space OK. No missing tool.

---
## BENCHMARK + HOTSPOTS — RUN #3 (2026-09-17 ~23:30, post PivotFloorFactor=1e-2 fix)

Accuracy gate (n=4001, dense Matern Cholesky reference, load-independent):
  relerr = 4.75e-3  < 1e-2  ->  HEALTHY  (was 5.13 pre-fix)
  shift  = 1.72e-2 (heal band)  ; no nested-failure WRN
Benchmark n=20002 (load-polluted ~1.2-3.3, so times are UPPER bounds):
  factorize = 2.2 s   solve = 9 ms   y0 = 2.97, ylast=3.6e-2, shift 1.7e-2
  (pre-fix corrupt: 21.7 s factorize / H-err 5.3)

Hotspots (perf 6.18 callgraph, CPU atom cycles; + cachegrind n=801/n=4001):
  blas_thread_server (OpenBLAS threaded DTRSM/GEMM/DPOTRF on dense children) 44.7%
  __expm1l 19.1%  __expl 8.4%  rem_pio2 3.5%   <- Matern kernel eval in
                                                  assembly/recompression
  std::sort (recompressLowRank index sort) ~9%
  -> BLAS dense-child factorize + Matern exp = intrinsic hotspot; NOT the heal loop.

Tools: valgrind 3.24 (cachegrind/callgrind ✓), perf 6.18 with call-graph ✓.
No missing tool needed to be requested.

---
## IDLE-GATED CLEAN RE-BENCH (2026-09-17 04:10, load 0.14-0.21 sustained idle)
[ACC] n=4001  factorize=0.192s  H-err=4.747e-03  PASS   (<1e-2, matches golden ~1e-3 band)
[TIME] n=20002 factorize=1.809s solve=0.0081s y0=2.966  shift=1.718e-02
  -> idle-box time ~1.8s (still not the 120ms pre-session, but CORRECT at 4.7e-3;
     the mandate is slow-but-correct). HEAL=1.72e-2 in the healthy pivot band.

---
## SESSION RECOVERY (2026-09-20/21) — floor reverted to 1e-4, all committed tests green

Interrupted session recovered on branch `hodlr` (HEAD `7d250411f`, 3 commits ahead
of branch base `f2d1ff919`). Working tree only carried the uncommitted
pivot-floor heal fix (HODLRCore.cxx/HODLRMatrixImplementation.cxx/HODLRCore.hxx,
ResourceMap key, openturns.conf.in, t_HODLRMatrix_std.py). No commit from the
whole HODLR-fix campaign; everything validated in-tree.

**Gate finding (deterministic, load-independent): the tune to PivotFloorFactor=1e-2
is WRONG.** It regresses committed test 25 (ACA solve, n=2000 Matern corr=0.1
leaf=16, nugget=0): heal escalates to shift=512, err 1.24e5 (assert <1e-2).
At floor 1e-4 the SAME case heals to shift 1.34e-4, err 8e-4. Test 30
(near-singular heal regression) at 1e-4: factorize 289ms, shift 1.34e-4, err
5.3e-15. FULL t_HODLRMatrix_std 30/30 PASS at floor 1e-4.
=> Reverted HODLRMatrix-PivotFloorFactor 1.0e-2 -> 1.0e-4 in
   lib/src/Base/Common/ResourceMap.cxx AND lib/etc/openturns.conf.in.
   (User-confirmed decision; the 1e-2 tune bought n=20002 factorize speed at the
   price of a broken committed test -> "slow but correct".)

**OPEN (deferred to an idle window, load was 60-77 from an unrelated
openturns-manifold pyinstallcheck):** current tree does NOT reproduce the
09-17 golden for corr=0.7 n=4001/n=20002 leaf=250: measured shift ~1.0e3
(near corruptionCap=1000*kernelDiagMax) and err ~1.0 at BOTH floors, where the
golden harness recorded shift~1.7e-2 / err 4.7e-3. The golden fixture
(/tmp/hodlr_acc_time.py) is gone (prior host). Suspect: a regression introduced
after the 09-17 04:10 re-bench, or a fixture difference. To re-derive on idle:
run n=4001 corr=0.7 leaf=250 err-vs-dense under the current tree and the
git-HEAD build to isolate the delta.

**Machine/build state (AGENTS_local.md-compliant):**
- Build dir $HOME/tmp/hodlr/ (fresh strict configure: RelWithDebInfo, Werror
  union, Unity ON b32, MOLD, SWIG -Werror, USE_HMAT=ON, USE_IPOPT=OFF
  USE_BONMIN=OFF because the local Ipopt 3rd-party headers trip
  -Wsuggest-override/-Werror in Bonmin.cxx; user chose to disable the solvers
  rather than touch tracked sources).
- Install prefix $HOME/tmp/hodlr/install, PYTHONPATH per-invocation.
- `git config core.filemode false` locally: the 2026-09-20 directory copy
  flipped 6411 files to 100755 (mode-only noise, zero content).

**Status:** HODLR pyinstallcheck subset 6/6 green at floor 1e-4
(HODLRMatrix_std, leafsize, fallback, GaussianProcess_std_hodlr,
GaussianProcessFitter_std_hodlr, cantilever_beam_hodlr example). Full
`ctest -R pyinstallcheck` gate running (detached). Next: flake8/doc8, then
commit the HODLR fix (lib/ + python/ only; session artifacts stay untracked),
then plan the upstream/master sync (7 commits ahead: Simulation bridge, SIR,
skills files; master_changed.txt record in /tmp/opencode/).

---
## HEAL FIX COMMITTED + REBASE ONTO UPSTREAM/MASTER (2026-09-21)

**1. Validation (pre-rebase):** full pyinstallcheck gate 100% passed (1035/1036,
1 = pyinstallcheck_example_plot_optimization_bonmin disabled on our build),
flake8 + doc8 green. Added the HODLRMatrix-PivotFloorFactor key to the
HODLRMatrixImplementation docstring Notes (ResourceMap-missing test requires
it), ChangeLog API-change line for the heal, rebuilt + reinstalled.

**2. Commit:** `f2e717f2f` "HODLR: heal near-singular leaf blocks with a local
pivot-floor shift" (8 tracked files, +408/-80; session artifacts untracked).

**3. Branch had FIVE local commits** (not 4 as recovered): below the HODLR stack
sits `70ec97ebe CovarianceModel: optimize kernel evaluation and discretization
(replayed as c0dd7fc21)`.

**4. Rebase onto upstream/master (user chose rebase; ChangeLog manual merge
only):** new history = [upstream c0dd7fc21..f5f8d2076, 8 commits] + replayed
CovarianceModel(HODLR base), HODLR ba1f5aa86, HMatrix 4bbc88cb1, Benchmarks
f66800be8, heal fix 63b64ff98. Content parity verified: diffs vs the old
pre-rebase commit hashes are exactly the 108-file upstream delta. Merge-base
with upstream/master = f5f8d2076; tree parity confirmed.

**5. Post-rebase rebuild + validation:** deleted build python/, full
build+install green under strict flags; flake8/doc8 green. Full gate
(log /tmp/opencode/gate_rebase.log) running. Remaining: confirm gate, then
idle-gated re-bench for corr=0.7 (OPEN, above) if load allows.

---
## ACCURACY AUDIT vs LAPACK (2026-09-21, load-independent) — corr=0.7 is NOT the only issue

User asked to hunt ALL accuracy issues with dense LAPACK as reference before
any performance work. Harness /tmp/opencode/acc_sweep.py (sweep 1D [0,1]
Matern(2.5) + Exponential, n={1000,2000,4001}, corr={0.01..1.0}, leaf={16,64,250},
nugget=0; metrics: rel_solve vs dense LAPACK solveLinearSystem, rel_matvec =
||A_H v - A v||/||A v|| probe, shift, power-iter ||A-A_H||_2 cross-check).
Results in /tmp/opencode/acc_sweep.csv.

### Exponential: PERFECT everywhere (max solverr 2.9e-10, max mverr 1.3e-12,
never heals). The defect is 100% Matern(2.5)-specific.

### Matern accuracy map (HARD = rel_matvec >= 1e-2, PROVEN operator corruption
via power-iter; SOFT = A_H~A but rel_solve >= 1e-2, near-singular ref regime;
"." = healthy <1e-2 both metrics):
```
        leaf  16        64       250
corr 0.01   . . .     . . .     . . .        (n=1000/2000/4001)
corr 0.05   . . .     . . .     . . .
corr 0.1    . . HARD  . . SOFT  . . .
corr 0.3    . HARD HARD . SOFT SOFT . . .
corr 0.5    HARD HARD HARD . SOFT SOFT . . .
corr 0.7    HARD HARD HARD . HARD HARD . . HARD
corr 1.0    HARD HARD HARD SOFT HARD HARD . SOFT HARD
            (leaf16: n1000,2000,4001 | leaf64: n1000,2000,4001 | leaf250: ...)
```
- leaf=16 corrupts at corr>=0.1 (n>=2000) and corr>=0.5 (n=1000): the config
  used by t_HODLRMatrix_std (corr=0.1 n=2000 leaf=16) is BARELY healthy.
- leaf=64 corrupts at corr>=0.5.
- leaf=250 (DEFAULT MinLeafSize) corrupts at corr>=0.7 n=4001 — the known
  golden case — and at corr=1.0 n=4001.
- Power-iteration cross-check: corr=0.5 n=1000 leaf=16 ||A-A_H||2/||A||2 = 16.9x,
  corr=0.1 n=4001 leaf=16 = 12.9x; shifts stay TINY (1e-6..1.3e-4) => the heal
  does NOT escalate, the fail-safe silently accepts corrupt factors.
  Controls at leaf=250 (corr=0.1/0.5): rel 1e-7, healthy.

### GP fitter end-to-end (LAPACK vs HODLR, optimize OFF, Matern2.5, constant basis):
  corr=0.1 n=2000 (default leaf250, matrix-clean 1e-6): loglik diff -1.5e4,
     pred mean rel diff 0.25     <- default-config production case diverges!
  corr=0.7 n=2000: loglik -1.9e4, pred 1.5x
  corr=0.7 n=4001: loglik -3.8e4, pred 3.5x
  corr=1.0 n=4001: loglik -3.8e4, pred 0.75x
  => even where the operator is norm-clean (mverr~1e-6, corr>=0.3 cond>=1e12),
     the GP fitter output is knife-edge: O(1e-6) operator perturbations x
     cond(>=1e12) -> O(0.25-3.5) predictive-mean and O(1e4) likelihood diffs.
  For a fair reference the near-singular GP itself must be regularized/nuggeted
  identically on both backends; today the reference is raw dense Cholesky.

### Takeaways
1. The committed test suite OWNS the healthy corner only; the Matern corruption
   band (leaf small OR corr>=0.5 at any leaf) is untested in python/test.
2. Root-cause direction (matches PLAN "Suspect 1/4"): the heal acceptance
   (minPivot^2 >= max(PivotFloorFactor*kernelDiagMax,1e-12)) terminates at the
   1e-4 floor for leaf<=64 (accepts corrupt L), while at large n the corrected-
   dense leaf path (s1<=256) cascades. Single global floor 1e-2 "fixes" corr=0.7
   but breaks test 25 => the acceptance rule needs a LOCAL/block-relative
   criterion, not a global floor. AXIS: be bullet-proof on accuracy first.
3. Deferred: exponential timing; nothing to re-bench yet.

NEXT: diagnose the leaf-corruption mechanism (instrument heal acceptance per
leaf on corr=0.5 n=2000 leaf=16, the smallest HARD cell) BEFORE any
performance work.

---
## ACCURACY MECHANISM DIAGNOSIS (2026-09-21) — nested near-cancellation cascade

Instrumented (HODLR_LEAF_DEBUG, already compiled) corr=0.5 n=1000 leaf=16
(corrupt) vs leaf=250 (healthy control) Matern2.5, + dense reconstruction of
A_H by basis-probe gemv with blockwise diff vs A.

### Where the factor is wrong (blockwise, corrupt cell)
- Leaves 0..30 (child0): relF <= 1e-15 (PERFECT).
- Leaves 31..61 (child1, the SECOND subtree): relF = 2050x EVERYWHERE.
- Off-diagonal blocks: relF 7-15. Global ||A_H - A||_F/||A||_F = 242.
=> the corruption is entirely ONE child subtree, fed by the sibling's
   Schur-complement correction.

### Evidence chain (corrupt cell)
- Bottom internal nodes (size=62) carry kMax = W^T W up to 3.19e5 (i.e.
  ||W|| ~ 5.6e2) and ukMax (correction magnitude) up to 8.6e3.
- The corrected LEAF blocks collapse: maxDiag = 8e-4 .. 6e-2 (vs raw kernel
  diag 1.0), and the heal "accepts" them at heal = 2e-12 because
  minPivot^2 ~ 1e-3 already exceeds the floor max(1e-4*kernelDiagMax,1e-12).
- Healthy control leaf=250: bottom ukMax <= 9, corrected leaf collapses stay
  mild, heal 1.3e-4 on at most one leaf, all blocks relF ~ 1e-7.

### Mechanism (matches PLAN "smoking-gun" from 09-16, now generalized)
1. Down the tree a leaf's L00 ends with small pivots ~1e-2 (heal accepted at
   the 1e-4 floor relative to RAW kernel diag -- the WRONG reference scale).
2. W = L00^-1 V -> O(1e2..1e3);  K = W^T W -> O(1e2..1e5);  the correction
   U*K*U1^T -> ukMax ~ 1e3..8e3.
3. Applying that correction to the sibling child's leaves cancels their
   diagonals by 1e4 (1.0 -> 1e-4..1e-3): near-cancellation collapse.
4. The heal then ACCEPTS a collapsed leaf at heal~2e-12 (block pivot already
   >= floor) even though its factor is dominated by cancellation roundoff.
5. The entire child1 subtree reconstructs to A_H ~ 2000x A; solve returns
   garbage silently; no failure is raised.

### Two failure routes, one root
- Route A (current floor 1e-4): under-heal -> W/K/ukMax unwind -> collapse ->
  silent corruption (the sweep's HARD band).
- Route B (tune 1e-2): over-escalation on test25 -> heal chases a
  collapse-corrupted block up to shift 512, minPivot^2 >= 1e-2 finally met
  with the block ~ lambda*I -> factor garbage (err 1.24e5).
A single global pivot floor CANNOT cure both. The discriminator between the
golden healthy heal (0.034 -> minPivot ~0.18 -> ||L00^-1|| ~ 6, corrections
mild) and Route B is NOT the floor value but whether ||W|| = ||L00^-1 V||
stays bounded.

### Candidate fix (to validate before implementing)
Heal target := keep W = L00^-1 V bounded (e.g. raise heal until
minPivot >= c * ||V||_rowmax / budget, budget ~ 1-10), OR diagnose the
collapse directly: after computing pristine_ on a leaf, if its corrected
diagonal maxDiag < epsCollapse * kernelDiagMax (collapse ratio ~1e-4..1e-3),
the leaf is the victim of an upstream W-corruption -> throw to the OUTER
loop for a global shift (which cures every block at once), instead of the
leaf heal accepting near-cancellation. Validate on: test25 (corr=0.1 n=2000
leaf=16, MUST stay err<1e-2 AND heal small), corr=0.5 n=1000 leaf=16 (HARD,
must go healthy), corr=0.7 n=4001 leaf=250 (golden, must reproduce
4.7e-3/1.7e-2 band), full 126-cell accuracy sweep (must clear all HARD),
then the committed 30/30 HODLR tests + gates.

Reference metrics: rel_matvec/blockwise relF (corrupt signature = one child
subtree relF 1e3+), power-iter ||A-AH||2, solve rel-err; reload-independent.

## 09-21 session: ROOT CAUSE of the heal/pivot-floor never firing on attempt 0

Instrumented HODLRCore.cxx (env-gated HODLR_LEAF_DEBUG) on the corrupt cell
(corr=0.5 n=1000 leaf=16) and found:

- Leaf start=0 (the near-rank-1 block [0,31), kernel k01=0.9999967, eigmin
  ~2e-14): printed minPivot^2=1.0 and heal=0 att=0 (accepted), while the
  TRUE factor (a fresh independent loop over the SAME buffer at the same
  event) had min(L diag)=4.266e-6 => true minPivot^2 = 1.8e-11.
- A pointer-identity probe settled it: &Sfact[0] != &(*Sfactor_)[0]
  (ptrEqual=0), Sfact960=1.0 (stale pre-dpotrf content) vs sf960=4.266e-6
  (the real factor). The pivot-floor loop was reading a STALE buffer.

### Why: copyOnWrite() detaches Sfactor_ under the dpotrf
- factorizeLeafCholesky assigns Sfactor_ = unregularized at 1398, which
  SHARES the leafKernel-derived buffer with pristine_ (refcount 2).
- On attempt 0 the "Sfactor_ = Matrix(*pristine_)" clone at line 1448 is
  SKIPPED (guarded by attempt>0). So at attempt 0, Sfactor_ still shares
  pristine_'s buffer and the reference "MatrixImplementation& Sfact =
  *Sfactor_.getImplementation()" binds to that SHARED buffer.
- The dpotrf is called with &Sfactor_(0,0). Matrix::operator() runs
  copyOnWrite(): because the impl is NOT unique (shared with pristine_), it
  resets Sfactor_ to a FRESH clone and returns a ref into the NEW buffer.
  dpotrf then writes the Cholesky factor into the NEW buffer, but Sfact
  still points at the OLD shared (pristine) buffer whose diagonal is the
  kernel diagonal (1.0).
- minPivot therefore measured the INPUT diagonal (1.0), not the factor
  pivots: healOK = (1.0 >= 1e-4) is ALWAYS true on attempt 0 -> the
  pivot-floor heal loop is dead on the first attempt and silently accepts
  near-singular factors. On attempt>0 the clone makes Sfactor_ unique, so
  healing (when reached after an info>0 failure) did read the real pivots.

### One-line fix + measured effect (NOT committed)
Replace "if (attempt > 0) Sfactor_ = Matrix(*pristine_)" with an
unconditional "Sfactor_ = Matrix(*pristine_)" so attempt 0 already works on
a leaf-private buffer and minPivot reads the true Cholesky pivots.

- corr=0.5 n=1000 leaf=16 (was HARD corrupt, relF 242, ||A-AH||2 ~16.9x):
  relNorm2 = 7.0e-4 (HEALTHY). Leaf0 heals to 1.342e-4, att=27, only ONE
  leaf healed.
- corr=0.5 n=1000 leaf=250 healthy control: relNorm2 = 6.7e-4 (unaffected).
- COMMITTED test25 (corr=0.1 n=2000 leaf=16) REGRESSES: solve err 0.73
  (had <1e-2). The leaf heal 1.34e-4 sits on the near-null eigenmode of K
  (predominantly localized in leaf0), so it perturbs the solve ~lambda/eigmin
  while leaving the spectral norm tiny.

### Conclusion for the next step
The pivot floor is now HONEST but the local leaf heal is the wrong tool for
near-rank-1 leaves: it fixes ||A-AH||2 but breaks the solve. Revert this
branch to the green pushed state; pursue instead the validated candidate:
detect the COLLAPSE (a leaf whose corrected diagonal maxDiag collapses
~1e-4..1e-3 below kernelDiagMax is the victim of an upstream W-corruption ->
escalate a GLOBAL shift via the outer loop, never a local leaf heal) or keep
W = L00^-1 V bounded. Revalidate on test25 + corr=0.5 corrupt + corr=0.7
golden + 126-cell sweep + 30/30 committed HODLR tests once chosen.
