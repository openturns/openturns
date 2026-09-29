%feature("docstring") OT::SCIP
R"RAW(Mixed-integer nonlinear optimization solver.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

This class exposes the mixed-integer nonlinear solver from the `SCIP <https://scipopt.org/>`_ library.
It handles continuous, integer and binary variables with linear or nonlinear
objective and constraints. Linear functions are passed as linear constraints
while other functions go through a black-box expression handler.

Parameters
----------
problem : :class:`~openturns.OptimizationProblem`
    The problem

Notes
-----
One iteration of the solver is one branch-and-bound node, so the number of nodes is
limited by :meth:`~openturns.experimental.SCIP.setMaximumIterationNumber`, which defaults
to the ``SCIP-DefaultMaximumIterationNumber`` ResourceMap entry. The variable bounds
must be tight: they are the only domain information available to bound a black-box function,
and a variable left free over a huge interval cannot be bounded by the solver.

The following :class:`~openturns.ResourceMap` key is used:

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - Key
     - Description
   * - ``SCIP-DefaultMaximumIterationNumber`` (``UnsignedInteger``, default: ``10000``)
     - Default number of branch-and-bound nodes.

Black-box objective functions are handled through an auxiliary variable, which is bounded
using the range of the objective over the domain. Reaching the global minimum of a
nonconvex black-box function is not guaranteed, as it depends on the nonlinear capabilities
of the underlying SCIP build (an NLP solver such as Ipopt must be linked in for the best
results), and the returned solution may be a local minimum.

SCIP solver parameters can be modified through the :class:`~openturns.ResourceMap`.
For every option ``optionName``, simply add a key named ``SCIP-optionName`` with the value to use, as shown below::

    >>> import openturns as ot
    >>> ot.ResourceMap.AddAsScalar('SCIP-limits/time', 60.0)
    >>> ot.ResourceMap.AddAsBool('SCIP-display/verblevel', False)

The ``SCIP-DefaultMaximumIterationNumber`` key is an exception, it is not passed to SCIP
but sets the default number of nodes, that is the default value of the
:attr:`~openturns.experimental.SCIP.maximumIterationNumber` attribute::

The capabilities of the linked SCIP library can be checked with the
:meth:`~openturns.experimental.SCIP.GetLPsolver` and
:meth:`~openturns.experimental.SCIP.GetNLPsolvers` static methods. They respectively
give the LP solver used to solve the linear relaxations and the NLP solvers available
to handle the nonlinear parts. An NLP solver can be selected using the ``nlp/solver``
option, and the NLP relaxation can be turned off using the ``nlp/disable`` option::

    >>> import openturns as ot
    >>> import openturns.experimental as otexp
    >>> nlpSolvers = otexp.SCIP.GetNLPsolvers()
    >>> ot.ResourceMap.AddAsString('SCIP-nlp/solver', 'ipopt')
    >>> ot.ResourceMap.AddAsBool('SCIP-nlp/disable', True)

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> objective = ot.SymbolicFunction(['x0', 'x1'], ['x0^2 + x1^2'])
>>> bounds = ot.Interval([0.0, 0.0], [5.0, 5.0])
>>> problem = ot.OptimizationProblem(objective)
>>> problem.setBounds(bounds)
>>> algo = otexp.SCIP(problem) # doctest: +SKIP
>>> algo.run() # doctest: +SKIP
>>> result = algo.getResult() # doctest: +SKIP
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
