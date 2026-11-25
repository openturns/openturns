%feature("docstring") OT::SCIP
R"RAW(Mixed integer nonlinear optimization solver.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.
    It also requires OpenTURNS to be built with the optional
    `SCIP <https://www.scipopt.org/>`_ library.

This class exposes the MINLP solver from the SCIP library.
It solves continuous, integer and binary problems with linear and nonlinear
objective and constraints, using a spatial branch-and-bound algorithm.
Least squares and nearest point problems are not supported.

Parameters
----------
problem : :class:`~openturns.OptimizationProblem`, optional
    Optimization problem to solve. Default is an empty problem.

Notes
-----
The objective and constraint functions are passed to SCIP as black-box
callbacks. Their values and gradients, either provided by the user or computed
by finite differences, are provided to SCIP.

The optimization limits are mapped to native SCIP limits whenever possible:

* :meth:`~openturns.OptimizationAlgorithm.setMaximumTimeDuration` maps to
  the ``limits/time`` parameter;
* :meth:`~openturns.OptimizationAlgorithm.setMaximumIterationNumber` maps to
  the ``limits/nodes`` parameter;
* :meth:`~openturns.OptimizationAlgorithm.setMaximumConstraintError` maps to
  the ``numerics/feastol`` parameter;
* :meth:`~openturns.OptimizationAlgorithm.setMaximumCallsNumber` is enforced
  cooperatively: the solve is interrupted as soon as the budget of objective
  and constraint evaluations is exhausted.

SCIP can be adapted using the numerous parameters described in its
`online documentation <https://www.scipopt.org/doc/html/parameters.php>`_.
These parameters can be modified through the :class:`~openturns.ResourceMap`.
For every option ``optionName``, simply add a key named ``SCIP-optionName``
with the value to use, as shown below::

    >>> import openturns as ot
    >>> ot.ResourceMap.AddAsUnsignedInteger('SCIP-limits/solutions', 10)
    >>> ot.ResourceMap.AddAsScalar('SCIP-limits/gap', 1e-4)

The following :class:`~openturns.ResourceMap` keys set the default budgets,
they are not forwarded to SCIP:

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - Key
     - Description
   * - ``SCIP-DefaultMaximumCallsNumber`` (``UnsignedInteger``, default: ``1000000``)
     - Default number of function evaluations.
   * - ``SCIP-DefaultMaximumIterationNumber`` (``UnsignedInteger``, default: ``10000``)
     - Default number of branch-and-bound nodes.

The capabilities of the linked SCIP library can be checked with the
:meth:`~openturns.experimental.SCIP.GetLPsolver` and
:meth:`~openturns.experimental.SCIP.GetNLPsolvers` static methods. They
respectively give the LP solver used for the linear relaxations and the NLP
solvers available for the nonlinear parts. An NLP solver can be selected using
the ``nlp/solver`` option, and the NLP relaxation can be disabled using the
``nlp/disable`` option::

    >>> import openturns.experimental as otexp
    >>> nlpSolvers = otexp.SCIP.GetNLPsolvers()
    >>> ot.ResourceMap.AddAsString('SCIP-nlp/solver', 'ipopt')
    >>> ot.ResourceMap.AddAsBool('SCIP-nlp/disable', True)

The nonlinear objective is reformulated with an auxiliary variable ``z`` and
the constraint ``z = f(x)``. Black-box functions are not interval-evaluable,
so SCIP cannot build a rigorous relaxation of a nonlinear term and cannot
prove optimality for a nonlinear problem: the solve stops on the node budget
with the best incumbent. When no feasible point has been found, the solve
reports a failure and the optimal point is empty. Linear and mixed-integer
linear problems are solved to optimality.

See Also
--------
openturns.Bonmin, openturns.OptimizationProblem

Examples
--------
The code below ensures the optimization of the following problem:

.. math:: min \left( - x_0 - x_1 - x_2 \right)

subject to

.. math::

    \left(x_1 - \frac{1}{2}\right)^2 + \left(x_2 - \frac{1}{2}\right)^2 \leq \frac{1}{4}

    x_0 - x_1 \leq 0

    x_0 + x_2 + x_3 \leq 2

    x_0 \in \{0,1\}

    (x_1, x_2) \in \mathbb{R}^2

    x_3 \in \mathbb{N}

>>> import openturns as ot
>>> import openturns.experimental as otexp

>>> # Definition of objective function
>>> objectiveFunction = ot.SymbolicFunction(['x0','x1','x2','x3'], ['-x0 -x1 -x2'])

>>> # Definition of variables bounds
>>> # The disk constraint implies x1 <= 1 and x2 <= 1: keep finite bounds,
>>> # they are the only domain information a black-box solver can use.
>>> bounds = ot.Interval([0,0,0,0],[1,1,1,5],[True,True,True,True],[True,True,True,True])

>>> # Definition of variables types
>>> variablesType = [ot.OptimizationProblemImplementation.BINARY,
... ot.OptimizationProblemImplementation.CONTINUOUS,
... ot.OptimizationProblemImplementation.CONTINUOUS,
... ot.OptimizationProblemImplementation.INTEGER]

Inequality constraints are defined by a function :math:`h` such that :math:`h(x) \geq 0`.

>>> # Definition of constraints
>>> h = ot.SymbolicFunction(['x0','x1','x2','x3'], ['-(x1-1/2)^2 - (x2-1/2)^2 + 1/4', '-x0 + x1', '-x0 - x2 - x3 + 2'])

>>> # Setting up the problem
>>> problem = ot.OptimizationProblem(objectiveFunction)
>>> problem.setBounds(bounds)
>>> problem.setVariablesType(variablesType)
>>> problem.setInequalityConstraint(h)

>>> scipAlgorithm = otexp.SCIP(problem)
>>> scipAlgorithm.setStartingPoint([0,0,0,0])
>>> scipAlgorithm.setMaximumCallsNumber(10000)
>>> scipAlgorithm.setMaximumIterationNumber(1000)

>>> # Running the solver
>>> scipAlgorithm.run() # doctest: +SKIP

>>> # Retrieving the results
>>> result = scipAlgorithm.getResult() # doctest: +SKIP
>>> optimalPoint = result.getOptimalPoint() # doctest: +SKIP
>>> optimalValue = result.getOptimalValue() # doctest: +SKIP
>>> evaluationNumber = result.getInputSample().getSize() # doctest: +SKIP
)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::SCIP::GetLPsolver
"Accessor to the LP solver name.

Returns
-------
lpsolver : str
    Name and version of the LP solver embedded in the SCIP library, for
    instance ``SoPlex 8.1.0``. Empty if the SCIP library has no LP solver.

Examples
--------
>>> import openturns.experimental as otexp
>>> print(otexp.SCIP.GetLPsolver()) # doctest: +SKIP
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SCIP::GetNLPsolvers
"Accessor to the NLP solvers names.

Returns
-------
nlpsolvers : :class:`~openturns.Description`
    Names of the NLP solvers embedded in the SCIP library, for instance
    ``['ipopt']``. Empty if the SCIP library has no NLP solver, in which case
    the nonlinear parts of a problem are only handled through their gradients.

Examples
--------
>>> import openturns.experimental as otexp
>>> otexp.SCIP.GetNLPsolvers() # doctest: +SKIP
"
