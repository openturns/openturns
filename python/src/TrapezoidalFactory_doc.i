%feature("docstring") OT::TrapezoidalFactory
"Trapezoidal factory.

Notes
-----
The parameters are estimated by the maximum likelihood method.

See also
--------
DistributionFactory, Trapezoidal"

// ---------------------------------------------------------------------

%feature("docstring") OT::TrapezoidalFactory::buildAsTrapezoidal
R"RAW(Estimate the distribution as native distribution.

**Available usages**:

    buildAsTrapezoidal()

    buildAsTrapezoidal(*sample*)

    buildAsTrapezoidal(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the :class:`~openturns.Trapezoidal`.

Returns
-------
dist : :class:`~openturns.Trapezoidal`
    The estimated distribution as a Trapezoidal.
    

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``TrapezoidalFactory-MaximumIteration`` (``UnsignedInteger``, default: ``2000``): maximum number of iterations of the optimization used to estimate the parameters
- ``TrapezoidalFactory-RhoBeg`` (``Scalar``, default: ``0.1``): initial value of the :math:`\rho` parameter of the trapezoidal distribution used as starting point of the optimization
- ``TrapezoidalFactory-RhoEnd`` (``Scalar``, default: ``1.0e-5``): final value of the :math:`\rho` parameter of the trapezoidal distribution used as bound of the optimization

In the first usage, the default Trapezoidal distribution is built.)RAW"

