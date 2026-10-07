%feature("docstring") OT::HiGHS
R"RAW(Linear optimization solver.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class exposes the linear solver from the `HiGHS <https://highs.dev/>`_ library.
It solves continuous linear problems and mixed-integer linear problems.

The recognized LP solver names are listed by :func:`GetAlgorithmNames`:

.. list-table::
   :widths: 15 55 30
   :header-rows: 1

   * - Solver
     - Description
     - Supported variables
   * - ``choose``
     - Default automatic choice: dual revised simplex for LP, branch-and-bound MIP for discrete problems.
     - Continuous and discrete
   * - ``simplex``
     - Primal/dual simplex, always returns a basic solution.
     - Continuous only
   * - ``ipm``
     - Interior point (HiPO or IPX depending on the HiGHS build).
     - Continuous only
   * - ``ipx``
     - Serial interior point solver.
     - Continuous only
   * - ``hipo``
     - Parallel interior point solver, best on large problems.
     - Continuous only
   * - ``pdlp``
     - First-order primal-dual hybrid gradient method (cuPDLP-C), no basic solution.
     - Continuous only
   * - ``hipdlp``
     - HiGHS native first-order primal-dual method, no basic solution.
     - Continuous only

See the `HiGHS documentation <https://ergo-code.github.io/HiGHS/>`_ for solver details.
Whether a name is accepted at runtime depends on the HiGHS build
(e.g. ``hipo`` requires a HiGHS build with HiPO support); an
unsupported name raises an error.

By default the interior point solvers run a crossover procedure to return
a basic solution; this can be tuned with the ``run_crossover`` option below.
The first-order solvers never return a basic solution.

After :meth:`run`, the HiGHS model status (``Optimal`` on success) is
available as the status message of the :class:`~openturns.OptimizationResult`,
whose status is set to ``SUCCESS``, ``FAILURE``, ``TIMEOUT``, ``INTERRUPTION``
or ``MAXIMUMCALLS`` accordingly.
Call ``setCheckStatus(False)`` before :meth:`run` to inspect a non-success
status instead of raising an exception.
On continuous problems with a valid dual solution, :func:`getDualPoint`
returns the constraint duals (shadow prices) and :func:`getReducedCosts`
the per-variable reduced costs; :func:`getConstraintValues` returns the
linear constraint activity row values.
There is no dual solution on problems with discrete variables.

HiGHS solver can be adapted using the parameters described `here <https://ergo-code.github.io/HiGHS/dev/options/definitions/>`_.
These parameters can be modified through the :class:`~openturns.ResourceMap`.
For every option ``optionName``, simply add a key named ``HiGHS-optionName`` with the value to use, as shown below::

    >>> import openturns as ot
    >>> ot.ResourceMap.AddAsBool('HiGHS-output_flag', True)
    >>> ot.ResourceMap.AddAsUnsignedInteger('HiGHS-threads', 4)

Parameters
----------
problem : :class:`~openturns.OptimizationProblem`
    The problem, must be linear (see :class:`~openturns.experimental.LinearProblem`).
algoName : str, default is ``choose``
    The LP solver, use :func:`GetAlgorithmNames` to list the recognized names.
    Problems with discrete variables are solved by the MIP solver,
    which ignores this choice: use ``choose`` on such problems.

See Also
--------
openturns.experimental.LinearProblem

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``HiGHS-solver`` (``String``, default: ``choose``): overrides the ``algoName`` constructor argument, use :func:`GetAlgorithmNames` to list the recognized names.
- ``HiGHS-run_crossover`` (``String``, default: ``on``): crossover of the interior point solvers, possible values are ``off``, ``choose`` and ``on``.
- ``HiGHS-mip_lp_solver`` (``String``, default: ``choose``): MIP subsolver choice on discrete problems, possible values are ``choose``, ``simplex``, ``ipm``, ``ipx`` and ``hipo``.
- ``HiGHS-mip_ipm_solver`` (``String``, default: ``choose``): MIP interior point subsolver choice on discrete problems, possible values are ``choose``, ``ipx`` and ``hipo``.
- ``HiGHS-mip_rel_gap`` (``Scalar``, default: ``1e-4``): relative MIP gap tolerance.
- ``HiGHS-mip_abs_gap`` (``Scalar``, default: ``1e-6``): absolute MIP gap tolerance.

Examples
--------
Solve a mixed integer linear optimization problem with objective:

.. math::

    f = 1.1 x_0 + x_1

with inequality constraints:

.. math::
    :nowrap:

    \begin{eqnarray*}
        & x_1 & \leq 7\\
        5 & \leq x_0 + 2x_1 & \leq 15\\
        6 & \leq 3x_0 + 2x_1 & \\
    \end{eqnarray*}

and bound constraints:

.. math::
    :nowrap:

    \begin{eqnarray*}
        0 & \leq x_0 & \leq 4\\
        1 & \leq x_1 &
    \end{eqnarray*}

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> cost = [1.1, 1.0]
>>> bounds = ot.Interval([0.0, 1.0], [4.0, 1e30])
>>> A = ot.Matrix([[0.0, 1.0], [1.0, 2.0], [3.0, 2.0]])
>>> cb = ot.Interval([-1e9, 5.0, 6.0], [7.0, 15.0, 1e9])
>>> problem = otexp.LinearProblem(cost, bounds, A, cb)
>>> problem.setVariablesType([ot.OptimizationProblemImplementation.INTEGER] * 2)
>>> algo = otexp.HiGHS(problem)
>>> algo.run() # doctest: +SKIP
>>> result = algo.getResult() # doctest: +SKIP
>>> x_star = result.getOptimalPoint() # doctest: +SKIP
>>> y_star = result.getOptimalValue() # doctest: +SKIP
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HiGHS::GetAlgorithmNames
"Accessor to the list of recognized LP solver names, by names.

Returns
-------
names : :class:`~openturns.Description`
    List of recognized LP solver names. Whether a name is accepted at
    runtime depends on the HiGHS build.

Examples
--------
>>> import openturns.experimental as otexp
>>> print(otexp.HiGHS.GetAlgorithmNames())
[choose,simplex,ipm,ipx,hipo,pdlp,hipdlp]"

// ---------------------------------------------------------------------

%feature("docstring") OT::HiGHS::setAlgorithmName
"Accessor to the LP solver name.

Parameters
----------
algoName : str
    The LP solver name, see :func:`GetAlgorithmNames`.
    Problems with discrete variables require ``choose``."

// ---------------------------------------------------------------------

%feature("docstring") OT::HiGHS::getAlgorithmName
"Accessor to the LP solver name.

Returns
-------
algoName : str
    The LP solver name."

// ---------------------------------------------------------------------

%feature("docstring") OT::HiGHS::getDualPoint
"Accessor to the constraint duals of the last run.

Returns
-------
duals : :class:`~openturns.Point`
    The linear constraint duals (shadow prices).

Notes
-----
Only available on continuous problems with a valid dual solution."

// ---------------------------------------------------------------------

%feature("docstring") OT::HiGHS::getReducedCosts
"Accessor to the reduced costs of the last run.

Returns
-------
costs : :class:`~openturns.Point`
    The per-variable reduced costs.

Notes
-----
Only available on continuous problems with a valid dual solution."

// ---------------------------------------------------------------------

%feature("docstring") OT::HiGHS::getConstraintValues
"Accessor to the linear constraint activity of the last run.

Returns
-------
values : :class:`~openturns.Point`
    The linear constraint row values.

Notes
-----
Only available after a run with linear constraints and a valid primal
solution."
