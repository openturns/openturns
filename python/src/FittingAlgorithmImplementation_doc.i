%define OT_FittingAlgorithm_doc
"Fitting algorithm.

Available constructors:
    FittingAlgorithm(*fittingAlgoImp*)

Parameters
----------
fittingAlgoImp : a FittingAlgorithmImplementation
    A fitting algorithm implementation which is the
    :class:`~openturns.CorrectedLeaveOneOut` or :class:`~openturns.KFold`.

See also
--------
CorrectedLeaveOneOut, KFold

Notes
-----
FittingAlgorithm is the interface of the FittingAlgorithmImplementation.
This class is not usable because it has sense only within the
:class:`~openturns.FunctionalChaosAlgorithm`."
%enddef
%feature("docstring") OT::FittingAlgorithmImplementation
OT_FittingAlgorithm_doc

// ---------------------------------------------------------------------

%define OT_FittingAlgorithm_run_doc
"Run the algorithm.

Usage:
  run(*x, y, weight, psi, indices*)

  run(*x, y, psi, indices*)

  run(*y, weight, indices, proxy*)

  run(*y, indices, indices*)

  run(method, y*)

With the first and second usages, we build the design proxy and apply the corresponding run.
With the third and fourth usages, we build a least square method with the proxy and right hand size.
With the fifth usage, we apply the fitting algorithm using an already defined least squares method.

Parameters
----------
x : 2-d sequence of float
    Input sample
y : 2-d sequence of float
    Output sample
weight : sequence of float
    Weights associated to the outputs
psi : sequence of :class:`~openturns.Function`
    Basis
indices : sequence of int
    Indices of the basis
proxy : :class:`~openturns.DesignProxy`
    The design proxy
method : :class:`~openturns.LeastSquaresMethod`
    Least square method (QR, SVD or Cholesky)

Returns
-------
measure : float
    Fitting measure"
%enddef
%feature("docstring") OT::FittingAlgorithmImplementation::run
OT_FittingAlgorithm_run_doc
