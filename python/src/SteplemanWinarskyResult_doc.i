%feature("docstring") OT::SteplemanWinarskyResult
R"RAW(Optimal step computed by :class:`~openturns.experimental.SteplemanWinarsky`,
with the model rebuilt with the finite-difference gradient.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
optimalStepCollection : 2-d sequence of float
    Optimal steps (one step per output).
functionWithOptimalStep : :class:`~openturns.Function`
    Model to differentiate, with its gradient replaced by a
    :class:`~openturns.CenteredFiniteDifferenceGradient` using the
    optimal step.

See Also
--------
openturns.experimental.SteplemanWinarsky
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarskyResult::getOptimalStep
R"RAW(Accessor to the optimal step.

Parameters
----------
marginalIndex : int, optional
    Index of the output marginal.
    Default is 0.

Returns
-------
optimalStep : :class:`~openturns.Point`
    Optimal finite-difference step for the selected marginal.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarskyResult::setOptimalStepCollection
R"RAW(Accessor to the optimal step.

Parameters
----------
optimalStepCollection : 2-d sequence of float
    Optimal finite-difference step for each output marginal.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarskyResult::getOptimalStepCollection
R"RAW(Accessor to the optimal step.

Returns
-------
optimalStepCollection : :class:`~openturns.Sample`
    Optimal finite-difference step for each output marginal.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SteplemanWinarskyResult::getFunctionWithOptimalStep
R"RAW(Accessor to the model with the finite-difference gradient.

Returns
-------
function : :class:`~openturns.Function`
    Model to differentiate, with its gradient replaced by a
    :class:`~openturns.CenteredFiniteDifferenceGradient` using the
    optimal step.
)RAW"
