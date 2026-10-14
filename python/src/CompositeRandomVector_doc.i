%feature("docstring") OT::CompositeRandomVector
"Random Vector obtained by applying a function.

Allows one to define the random variable :math:`Y=f(X)` from a function :math:`f`
and another random variable :math:`X`.

Parameters
----------
f : :class:`~openturns.Function`
    Function.
X : :class:`~openturns.RandomVector`
    Random vector :math:`\vect{X}`.

Notes
-----
The CompositeRandomVector is defined as the output through a model :math:`f`
of an input random vector :math:`\vect{X}`:

.. math::

    \vect{Y} = f(\vect{X})

The distribution of :math:`\vect{Y}` is not straightforward. The
CompositeRandomVector provides a method to generate some realizations, computed as the image
through :math:`f` of  some realizations of :math:`\vect{X}`.

If the random vector :math:`\vect{X}` is a :class:`~openturns.UsualRandomVector`, its
realizations are generated from its distribution.

If the random vector :math:`\vect{X}` is itself a :class:`~openturns.CompositeRandomVector`,
its realizations are generated using recursively its associated model and input random vector
until the initial random vector which is a :class:`~openturns.UsualRandomVector`.

In the library, random vectors are the inputs to :class:`~openturns.Functions`, unlike distributions, which cannot be used directly by functions.

Examples
--------
The random vector :math:`Y` is defined by :math:`Y = X^2 \sin X` where :math:`X` follows a standard normal distribution.

>>> import openturns as ot
>>> X = ot.RandomVector(ot.Normal())
>>> f = ot.SymbolicFunction(['x'], ['x^2*sin(x)'])
>>> Y = ot.CompositeRandomVector(f, X)

Draw a sample:

>>> sample = Y.getSample(5)"
