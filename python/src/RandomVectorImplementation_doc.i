%define OT_RandomVector_doc
"Random vectors.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    Distribution of the :class:`~openturns.UsualRandomVector` to define.

Notes
-----
A :class:`~openturns.RandomVector` models a vector that can take random values.
It is defined by a distribution or as the image of other random vectors through a function
:math:`f`.

In the library, random vectors are the inputs to :class:`~openturns.Function`, unlike distributions, which cannot be used directly by functions.

A random vector always provides a method to generate some realizations.

See also
--------
UsualRandomVector, CompositeRandomVector, CompoundRandomVector,
ConstantRandomVector, FunctionalChaosRandomVector, ThresholdEvent,
PythonRandomVector, RandomWalkMetropolisHastings"
%enddef
%feature("docstring") OT::RandomVectorImplementation
OT_RandomVector_doc

// ---------------------------------------------------------------------

%feature("docstring") OT::RandomVectorImplementation::getProcess
R"RAW(Get the stochastic process.

Returns
-------
process : :class:`~openturns.Process`
    Stochastic process used to define the random vector.

Notes
-----
The method applies only to random vectors :math:`\vect{Y}` which antecedent is a :class:`~openturns.Process`.
)RAW"

// ---------------------------------------------------------------------

%define OT_RandomVector_getAntecedent_doc
R"RAW(Accessor to the antecedent RandomVector in case of a composite RandomVector.

Returns
-------
antecedent : :class:`~openturns.RandomVector`
    Random vector :math:`\vect{X}` such that
    :math:`\vect{Y}=f(\vect{X})`.

Notes
-----
The method applies only to random vectors :math:`\vect{Y}` which are :class:`~openturns.CompositeRandomVector`.

If the antecedent :math:`\vect{X}` is itself a :class:`~openturns.CompositeRandomVector`, then its antecedent is searched, until the final antecedent which is a :class:`~openturns.UsualRandomVector`. The method returns this final random vector.
)RAW"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getAntecedent
OT_RandomVector_getAntecedent_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getFunction_doc
R"RAW(Accessor to the Function in case of a composite RandomVector.

Returns
-------
function : :class:`~openturns.Function`
    Function :math:`f` defining the :class:`~openturns.CompositeRandomVector`.

Notes
-----
The method applies only to random vectors :math:`\vect{Y}` which are :class:`~openturns.CompositeRandomVector` defined by :math:`\vect{Y}=f(\vect{X})`.

If the antecedent :math:`\vect{X}` is itself a :class:`~openturns.CompositeRandomVector`, then its antecedent is searched, until the final antecedent which is :class:`~openturns.UsualRandomVector`. Then, all the intermediate functions are composed. The method returns that final composed model.
)RAW"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getFunction
OT_RandomVector_getFunction_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getCovariance_doc
R"RAW(Accessor to the covariance of the RandomVector.

Returns
-------
covariance : :class:`~openturns.CovarianceMatrix`
    Covariance of the associated distribution.

Notes
-----
The method applies only to random vectors :math:`\vect{Y}` which are :class:`~openturns.UsualRandomVector` defined by a distribution.

Examples
--------
>>> import openturns as ot
>>> distribution = ot.Normal([0.0, 0.5], [1.0, 1.5], ot.CorrelationMatrix(2))
>>> randomVector = ot.RandomVector(distribution)
>>> ot.RandomGenerator.SetSeed(0)
>>> print(randomVector.getCovariance())
[[ 1    0    ]
 [ 0    2.25 ]]
)RAW"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getCovariance
OT_RandomVector_getCovariance_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getMean_doc
R"RAW(Accessor to the mean of the RandomVector.

Returns
-------
mean : :class:`~openturns.Point`
    Mean value of the associated distribution.

Notes
-----
The method applies only to random vectors :math:`\vect{Y}` which are :class:`~openturns.UsualRandomVector` defined by a distribution.

Examples
--------
>>> import openturns as ot
>>> distribution = ot.Normal([0.0, 0.5], [1.0, 1.5], ot.CorrelationMatrix(2))
>>> randomVector = ot.RandomVector(distribution)
>>> ot.RandomGenerator.SetSeed(0)
>>> print(randomVector.getMean())
[0,0.5]
)RAW"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getMean
OT_RandomVector_getMean_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getDistribution_doc
R"RAW(Accessor to the distribution of the RandomVector.

Returns
-------
distribution : :class:`~openturns.Distribution`
    Distribution that follows the random vector.

Notes
-----
The method applies only to random vectors :math:`\vect{Y}` which are :class:`~openturns.UsualRandomVector` defined by a distribution.

Examples
--------
>>> import openturns as ot
>>> distribution = ot.Normal([0.0, 0.0], [1.0, 1.0], ot.CorrelationMatrix(2))
>>> randomVector = ot.RandomVector(distribution)
>>> ot.RandomGenerator.SetSeed(0)
>>> print(randomVector.getDistribution())
Normal(mu = [0,0], sigma = [1,1], R = [[ 1 0 ]
 [ 0 1 ]])
)RAW"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getDistribution
OT_RandomVector_getDistribution_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getDimension_doc
"Accessor to the dimension of the RandomVector.

Returns
-------
dimension : positive int
    Dimension of the RandomVector."
%enddef
%feature("docstring") OT::RandomVectorImplementation::getDimension
OT_RandomVector_getDimension_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getMarginal_doc
R"RAW(Get the random vector corresponding to the :math:`i^{th}` marginal component(s).

Parameters
----------
i : int or list of ints, :math:`0\leq i < dim`
    Indicates the component(s) concerned. :math:`dim` is the dimension of the
    RandomVector.

Returns
-------
vector :  :class:`~openturns.RandomVector`
    RandomVector restricted to the concerned components.

Notes
-----
Let's note :math:`\vect{Y}=\Tr{(Y_1,\dots,Y_n)}` a random vector and
:math:`I \in [1,n]` a set of indices. If :math:`\vect{Y}` is a
:class:`~openturns.UsualRandomVector`, the subvector is defined by
:math:`\tilde{\vect{Y}}=\Tr{(Y_i)}_{i \in I}`. If :math:`\vect{Y}` is a
:class:`~openturns.CompositeRandomVector`, defined by
:math:`\vect{Y}=f(\vect{X})` with :math:`f=(f_1,\dots,f_n)`,
:math:`f_i` some scalar functions, the subvector is
:math:`\tilde{\vect{Y}}=(f_i(\vect{X}))_{i \in I}`.

Examples
--------
>>> import openturns as ot
>>> distribution = ot.Normal([0.0, 0.0], [1.0, 1.0], ot.CorrelationMatrix(2))
>>> randomVector = ot.RandomVector(distribution)
>>> ot.RandomGenerator.SetSeed(0)
>>> print(randomVector.getMarginal(1).getRealization())
[0.608202]
>>> print(randomVector.getMarginal(1).getDistribution())
Normal(mu = 0, sigma = 1)
)RAW"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getMarginal
OT_RandomVector_getMarginal_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getSample_doc
R"RAW(Compute realizations of the RandomVector.

Parameters
----------
n : int, :math:`n \geq 0`
    Number of realizations needed.

Returns
-------
realizations : :class:`~openturns.Sample`
    :math:`n` sequences of values of the random vector.

Notes
-----
The values are generated according to the type of the random vector.

In the case of a :class:`~openturns.ThresholdEvent`, the method returns :math:`n` realizations
of the event considered as a Bernoulli variable, which are boolean values: 1 for the
realization of the event and 0 else.

See also
--------
openturns.RandomVector.getRealization

Examples
--------
>>> import openturns as ot
>>> distribution = ot.Normal([0.0, 0.0], [1.0, 1.0], ot.CorrelationMatrix(2))
>>> randomVector = ot.RandomVector(distribution)
>>> ot.RandomGenerator.SetSeed(0)
>>> print(randomVector.getSample(3))
    [ X0        X1        ]
0 : [  0.608202 -1.26617  ]
1 : [ -0.438266  1.20548  ]
2 : [ -2.18139   0.350042 ])RAW"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getSample
OT_RandomVector_getSample_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getFrozenRealization_doc
"Compute realizations of the RandomVector.

In the case of a :class:`~openturns.CompositeRandomVector`
or an event of some kind, this method returns
the value taken by the random vector if the root cause
takes the value given as argument.

Parameters
----------
fixedPoint : :class:`~openturns.Point`
    Point chosen as the root cause of the random vector.

Returns
-------
realization : :class:`~openturns.Point`
    The realization corresponding to the chosen root cause.

See also
--------
openturns.RandomVector.getRealization
openturns.RandomVector.getFrozenSample

Examples
--------
>>> import openturns as ot
>>> distribution = ot.Normal()
>>> randomVector = ot.RandomVector(distribution)
>>> f = ot.SymbolicFunction('x', 'x')
>>> compositeRandomVector = ot.CompositeRandomVector(f, randomVector)
>>> event = ot.ThresholdEvent(compositeRandomVector, ot.Less(), 0.0)
>>> print(event.getFrozenRealization([0.2]))
[0]
>>> print(event.getFrozenRealization([-0.1]))
[1]"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getFrozenRealization
OT_RandomVector_getFrozenRealization_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getFrozenSample_doc
"Compute realizations of the RandomVector.

In the case of a :class:`~openturns.CompositeRandomVector`
or an event of some kind, this method returns
the different values taken by the random vector when the root cause
takes the values given as argument.

Parameters
----------
fixedSample : :class:`~openturns.Sample`
    Sample of root causes of the random vector.

Returns
-------
sample : :class:`~openturns.Sample`
    Sample of the realizations corresponding to the chosen root causes.

See also
--------
openturns.RandomVector.getSample
openturns.RandomVector.getFrozenRealization

Examples
--------
>>> import openturns as ot
>>> distribution = ot.Normal()
>>> randomVector = ot.RandomVector(distribution)
>>> f = ot.SymbolicFunction('x', 'x')
>>> compositeRandomVector = ot.CompositeRandomVector(f, randomVector)
>>> event = ot.ThresholdEvent(compositeRandomVector, ot.Less(), 0.0)
>>> print(event.getFrozenSample([[0.2], [-0.1]]))
    [ y0 ]
0 : [ 0  ]
1 : [ 1  ]"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getFrozenSample
OT_RandomVector_getFrozenSample_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getOperator_doc
"Accessor to the comparison operator of the Event.

Returns
-------
operator : :class:`~openturns.ComparisonOperator`
    Comparison operator of the event defining the random vector.

Notes
-----
The method applies only to :class:`~openturns.ThresholdEvent` random vectors."
%enddef
%feature("docstring") OT::RandomVectorImplementation::getOperator
OT_RandomVector_getOperator_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getThreshold_doc
"Accessor to the threshold of the Event.

Returns
-------
threshold : float
    Threshold of the event defining the random vector.

Notes
-----
The method applies only to :class:`~openturns.ThresholdEvent` random vectors."
%enddef
%feature("docstring") OT::RandomVectorImplementation::getThreshold
OT_RandomVector_getThreshold_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_isComposite_doc
"Accessor to know if the RandomVector is a composite one.

Returns
-------
isComposite : bool
    Indicates if the RandomVector is of type Composite or not."
%enddef
%feature("docstring") OT::RandomVectorImplementation::isComposite
OT_RandomVector_isComposite_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getDescription_doc
"Accessor to the description of the RandomVector.

Returns
-------
description : :class:`~openturns.Description`
    Describes the names of the random vector."
%enddef
%feature("docstring") OT::RandomVectorImplementation::getDescription
OT_RandomVector_getDescription_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_setDescription_doc
"Accessor to the description of the RandomVector.

Parameters
----------
description : str or sequence of str
    Describes the components of the RandomVector."
%enddef
%feature("docstring") OT::RandomVectorImplementation::setDescription
OT_RandomVector_setDescription_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getRealization_doc
"Compute one realization of the RandomVector.

Returns
-------
realization : :class:`~openturns.Point`
    One value of the random vector.

Notes
-----
The value is generated according to the type of the random vector.

In the case of a :class:`~openturns.ThresholdEvent`, the method returns one realization
of the event considered as a Bernoulli variable, which are boolean values: 1 for the
realization of the event and 0 else.

See also
--------
openturns.RandomVector.getSample

Examples
--------
>>> import openturns as ot
>>> distribution = ot.Normal([0.0, 0.0], [1.0, 1.0], ot.CorrelationMatrix(2))
>>> randomVector = ot.RandomVector(distribution)
>>> ot.RandomGenerator.SetSeed(0)
>>> print(randomVector.getRealization())
[0.608202,-1.26617]
>>> print(randomVector.getRealization())
[-0.438266,1.20548]"
%enddef
%feature("docstring") OT::RandomVectorImplementation::getRealization
OT_RandomVector_getRealization_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getDomain_doc
"Accessor to the domain of the Event.

Returns
-------
domain : :class:`~openturns.Domain`
    Describes the domain of an event.

Notes
-----
The method applies only to :class:`~openturns.ThresholdEvent` random vectors."
%enddef
%feature("docstring") OT::RandomVectorImplementation::getDomain
OT_RandomVector_getDomain_doc


// ---------------------------------------------------------------------

%define OT_RandomVector_setParameter_doc
"Accessor to the parameter of the distribution.

Parameters
----------
parameter : sequence of float
    Parameter values."
%enddef
%feature("docstring") OT::RandomVectorImplementation::setParameter
OT_RandomVector_setParameter_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getParameter_doc
"Accessor to the parameter of the distribution.

Returns
-------
parameter : :class:`~openturns.Point`
    Parameters of the distribution of the random vector.

Notes
-----
The method applies only to :class:`~openturns.UsualRandomVector` random vectors."
%enddef
%feature("docstring") OT::RandomVectorImplementation::getParameter
OT_RandomVector_getParameter_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_getParameterDescription_doc
"Accessor to the parameter description of the distribution.

Returns
-------
description : :class:`~openturns.Description`
    Parameter names of the distribution of the random vector.

Notes
-----
The method applies only to :class:`~openturns.UsualRandomVector` random vectors."
%enddef
%feature("docstring") OT::RandomVectorImplementation::getParameterDescription
OT_RandomVector_getParameterDescription_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_isEvent_doc
"Whether the random vector is an event.

Returns
-------
isEvent : bool
    Whether it takes it values in {0, 1}."
%enddef
%feature("docstring") OT::RandomVectorImplementation::isEvent
OT_RandomVector_isEvent_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_intersect_doc
"Intersection of two events.

Parameters
----------
event : :class:`~openturns.RandomVector`
    A composite event

Returns
-------
event : :class:`~openturns.RandomVector`
    Intersection event"
%enddef
%feature("docstring") OT::RandomVector::intersect
OT_RandomVector_intersect_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_join_doc
"Union of two events.

Parameters
----------
event : :class:`~openturns.RandomVector`
    A composite event

Returns
-------
event : :class:`~openturns.RandomVector`
    Union event"
%enddef
%feature("docstring") OT::RandomVector::join
OT_RandomVector_join_doc

// ---------------------------------------------------------------------

%define OT_RandomVector_asComposedEvent_doc
"If the random vector can be viewed as the composition of several :class:`~openturns.ThresholdEvent` objects,
this method builds and returns the composition.
Otherwise throws.

Returns
-------
composed : :class:`~openturns.RandomVector`
    Composed event."
%enddef
%feature("docstring") OT::RandomVectorImplementation::asComposedEvent
OT_RandomVector_asComposedEvent_doc
