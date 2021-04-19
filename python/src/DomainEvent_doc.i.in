%feature("docstring") OT::DomainEvent
"Event defined from a domain.

The event occurs when a realization of the underlying random vector belongs to the domain.

Parameters
----------
antecedent : :class:`~openturns.RandomVector` of dimension 1
    Antecedent.
domain : :class:`~openturns.Domain`
    Domain, of same dimension.

See also
--------
ProcessEvent, ThresholdEvent

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(6)
>>> dim = 2
>>> X = ot.RandomVector(ot.Normal(dim))
>>> model = ot.SymbolicFunction(['x1', 'x2'], ['x1+x2', '2*x1'])
>>> Y = ot.CompositeRandomVector(model, X)
>>> domain = ot.Interval(dim)
>>> event = ot.DomainEvent(Y, domain)
>>> sample = event.getSample(10)
>>> sample.setDescription([''])
>>> print(sample)
0 : [ 0 ]
1 : [ 0 ]
2 : [ 0 ]
3 : [ 0 ]
4 : [ 0 ]
5 : [ 0 ]
6 : [ 0 ]
7 : [ 1 ]
8 : [ 0 ]
9 : [ 0 ]"
