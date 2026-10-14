%feature("docstring") OT::UsualRandomVector
"Random Vector from a distribution.

Parameters
----------
X : :class:`~openturns.Distribution`
   Distribution of the random vector.

Notes
-----
The UsualRandomVector is defined by the distribution it follows. It provides a method to
generate some realizations which are generated using its distribution.

In the library, random vectors are the inputs to :class:`~openturns.Functions`, unlike distributions, which cannot be used directly by functions.

Examples
--------
This random vector is defined by the standard normal distribution.

>>> import openturns as ot
>>> X = ot.RandomVector(ot.Normal())

Draw a sample:

>>> sample = X.getSample(5)"
