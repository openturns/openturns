%feature("docstring") OT::ProductDistribution
R"RAW(Product distribution.

Distribution of the product :math:`Z=XY` of two independent univariate random variables :math:`X` and :math:`Y`.

We note :math:`Z=XY` the scalar random variable whose distribution has density :math:`f_Z`.
 
.. math::

    \displaystyle f_Z(z) = \int_{\Rset} f_X (t) f_Y(\dfrac{z}{t}) \, \dfrac{\Diff t}{|t|}

with :math:`f_X` and :math:`f_Y` the probability density functions of :math:`X` and :math:`Y`.
The cumulative distribution function is:

.. math::

    F_Z(z) = \int_{-\infty}^{0} f_X(t) S_Y\left(\dfrac{z}{t}\right) \Diff t
    + \int_{0}^{+\infty} f_X(t) F_Y\left(\dfrac{z}{t}\right) \Diff t

and its complementary counterpart is:

.. math::

    S_Z(z) = \int_{-\infty}^{0} f_X(t) F_Y\left(\dfrac{z}{t}\right) \Diff t
    + \int_{0}^{+\infty} f_X(t) S_Y\left(\dfrac{z}{t}\right) \Diff t

where :math:`F_Y` and :math:`S_Y` are the cumulative and complementary
cumulative distribution functions of :math:`Y`.

Its first moments, if finite, are defined as: 

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{Z} & = &  \Expect{X} \Expect{Y} \\
        \Var{Z} & = & \Expect{X}^2 \Var{Y} +  \Expect{Y}^2 \Var{X} + \Var{X}  \Var{Y} 
    \end{eqnarray*}

Parameters
----------

distX : :class:`~openturns.Distribution`, univariate
    Independent of `distY`

distY : :class:`~openturns.Distribution`, univariate
    Independent of `distX`

Returns
-------
distZ : :class:`~openturns.Distribution`, univariate
    :math:`distZ` is the distribution of the random variable :math:`Z = XY`
    when :math:`(X,Y)` is a random vector with independent copula
    and respective marginal distributions `distX` and `distY`.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distX = ot.Normal(-1.0, 1.0)
>>> distY = ot.Uniform(-1.0, 1.0)
>>> distZ = ot.ProductDistribution(distX, distY)

Draw a sample:

>>> sample = distZ.getSample(5)

Notes
-----
The following :class:`~openturns.ResourceMap` key is used:

- ``ProductDistribution-LargeCharacteristicFunctionArgument`` (``Scalar``, default: ``10``):
  if the modulus of the argument of the characteristic function is larger than
  this threshold, the factor characteristic function is integrated with an
  increased number of Gauss-Kronrod sub-intervals.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ProductDistribution::setLeft
"Accessor to the left distribution of the product.

Parameters
----------
distX :  :class:`~openturns.Distribution`."

// ---------------------------------------------------------------------

%feature("docstring") OT::ProductDistribution::setRight
"Accessor to the right distribution of the product.

Parameters
----------
distY :  :class:`~openturns.Distribution`."

// ---------------------------------------------------------------------

%feature("docstring") OT::ProductDistribution::getLeft
"Accessor to the left distribution of the product.

Returns
-------
distX :  :class:`~openturns.Distribution`."

// ---------------------------------------------------------------------

%feature("docstring") OT::ProductDistribution::getRight
"Accessor to the right distribution of the product.

Returns
-------
distY :  :class:`~openturns.Distribution`."
