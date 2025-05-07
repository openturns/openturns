%define OT_DistributionFactory_doc
"Base class for probability distribution factories.

Notes
-----
This class generally describes the factory mechanism of each OpenTURNS
distribution. Refer to :ref:`parametric_estimation` for information on the specific
estimators used for each distribution.
"
%enddef
%feature("docstring") OT::DistributionFactoryImplementation
OT_DistributionFactory_doc

// ---------------------------------------------------------------------

%define OT_DistributionFactory_build_doc
"Build the distribution.

**Available usages**:

    build()

    build(*sample*)

    build(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the distribution.

Returns
-------
dist : :class:`~openturns.Distribution`
    The estimated distribution.
    
    In the first usage, the default native distribution is built."
%enddef
%feature("docstring") OT::DistributionFactoryImplementation::build
OT_DistributionFactory_build_doc

// ---------------------------------------------------------------------

%define OT_DistributionFactory_buildEstimator_doc
"Build the distribution and the parameter distribution.

Parameters
----------
sample : 2-d sequence of float
    Data.
parameters : :class:`~openturns.DistributionParameters`
    Optional, the parametrization.

Returns
-------
resDist : :class:`~openturns.DistributionFactoryResult`
    The results.

Notes
-----
According to the way the native parameters of the distribution are estimated, the parameters distribution differs:

    - Moments method: the asymptotic parameters distribution is normal and estimated by Bootstrap on the initial data;
    - Maximum likelihood method with a regular model: the asymptotic parameters distribution is normal and its covariance matrix is the inverse Fisher information matrix;
    - Other methods: the asymptotic parameters distribution is estimated by Bootstrap on the initial data and kernel fitting (see :class:`~openturns.KernelSmoothing`).


If another set of parameters is specified, the native parameters distribution is first estimated and the new distribution is determined from it:

    - if the native parameters distribution is normal and the transformation regular at the estimated parameters values: the asymptotic parameters distribution is normal and its covariance matrix determined from the inverse Fisher information matrix of the native parameters and the transformation;
    - in the other cases, the asymptotic parameters distribution is estimated by Bootstrap on the initial data and kernel fitting.
"
%enddef
%feature("docstring") OT::DistributionFactoryImplementation::buildEstimator
OT_DistributionFactory_buildEstimator_doc

// ---------------------------------------------------------------------

%define OT_DistributionFactory_getBootstrapSize_doc
"Accessor to the bootstrap size.

Returns
-------
size : int
    Size of the bootstrap."
%enddef
%feature("docstring") OT::DistributionFactoryImplementation::getBootstrapSize
OT_DistributionFactory_getBootstrapSize_doc

// ---------------------------------------------------------------------

%define OT_DistributionFactory_setBootstrapSize_doc
"Accessor to the bootstrap size.

Parameters
----------
size : int
    The size of the bootstrap."
%enddef
%feature("docstring") OT::DistributionFactoryImplementation::setBootstrapSize
OT_DistributionFactory_setBootstrapSize_doc

// ---------------------------------------------------------------------

%define OT_DistributionFactory_setKnownParameter_doc
"Accessor to the known parameters.

Parameters
----------
positions : sequence of int
    Indices of known parameters.
values : sequence of float
    Values of known parameters.

Examples
--------
When a subset of the parameter vector is known, the other parameters only have to be estimated 
from data. 

In the following example, we consider a sample and want to fit 
a :class:`~openturns.Beta` distribution.  
We assume that the :math:`a` and :math:`b` parameters are known beforehand. 
In this case, we set the third parameter (at index 2) to -1 
and the fourth parameter (at index 3) to 1. 

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> distribution = ot.Beta(2.3, 2.2, -1.0, 1.0)
>>> sample = distribution.getSample(10)
>>> factory = ot.BetaFactory()
>>> # set (a,b) out of (r, t, a, b)
>>> factory.setKnownParameter([2, 3], [-1.0, 1.0])
>>> inf_distribution = factory.build(sample)"
%enddef
%feature("docstring") OT::DistributionFactoryImplementation::setKnownParameter
OT_DistributionFactory_setKnownParameter_doc

// ---------------------------------------------------------------------

%define OT_DistributionFactory_getKnownParameterValues_doc
"Accessor to the known parameters values.

Returns
-------
values : :class:`~openturns.Point`
    Values of known parameters."
%enddef
%feature("docstring") OT::DistributionFactoryImplementation::getKnownParameterValues
OT_DistributionFactory_getKnownParameterValues_doc

// ---------------------------------------------------------------------

%define OT_DistributionFactory_getKnownParameterIndices_doc
"Accessor to the known parameters indices.

Returns
-------
indices : :class:`~openturns.Indices`
    Indices of the known parameters."
%enddef
%feature("docstring") OT::DistributionFactoryImplementation::getKnownParameterIndices
OT_DistributionFactory_getKnownParameterIndices_doc

