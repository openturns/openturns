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
"Estimate the distribution as native distribution.

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
This class uses the following entries of :class:`~openturns.ResourceMap`:

- *TrapezoidalFactory-MaximumIteration* (:raw:`UnsignedInteger`, default: `2000`)
- *TrapezoidalFactory-RhoBeg* (:raw:`Scalar`, default: `0.1`)
- *TrapezoidalFactory-RhoEnd* (:raw:`Scalar`, default: `1e-05`)

    In the first usage, the default Trapezoidal distribution is built."

