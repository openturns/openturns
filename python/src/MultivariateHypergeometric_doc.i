%feature("docstring") OT::MultivariateHypergeometric
R"RAW(Multivariate hypergeometric distribution.

Randomizes drawing *N* balls without replacement from a finite population
of size *C* split into *d* tracked categories of sizes :math:`(c_1, \ldots, c_d)`
plus an implicit rest category of size :math:`C - \sum_{i = 1}^d c_i`.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
N : int, :math:`0 \leq N \leq C`
    The number of draws.

    Default value is 1.
colors : sequence of int, :math:`c_i \geq 0`, :math:`\sum_{i = 1}^d c_i \leq C`
    The population counts of each tracked category.

    Default value is [1].
total : int, :math:`C \geq \max(N, \sum_{i = 1}^d c_i)`
    The total population size.

    Default value is :math:`\sum_{i = 1}^d c_i` (classical case with empty rest).

Notes
-----

This distribution differs from the classical multivariate hypergeometric distribution definition.
The classical definition is constrained by :math:`C = \sum_{i = 1}^d c_i` and is supported by
:math:`\left\{\vect{x} \in \Nset^d\, |\, \sum_{i = 1}^d x_i = N, x_i \leq c_i\right\}`. The OpenTURNS distribution is
constrained by :math:`C \geq \sum_{i = 1}^d c_i` and is supported in general by
:math:`\cS=\left\{\vect{x} \in \Nset^d\, |\, \sum_{i = 1}^d x_i \leq N, x_i \leq c_i, N - \sum_{i = 1}^d x_i \leq C - \sum_{i = 1}^d c_i\right\}`.

Its probability distribution function is defined as:

.. math::

    \Prob{\vect{X} = \vect{x}} = \frac{\left[\prod_{i = 1}^d \binom{c_i}{x_i}\right] \binom{C - c}{N - s}}{\binom{C}{N}}\mathbf{1}_{\vect{x} \in \cS}

where :math:`c = \sum_{i = 1}^d c_i`, :math:`s = \sum_{i = 1}^d x_i`.

If :math:`C = c`, then the rest is empty and :math:`\sum_{i = 1}^d x_i = N`.
In this case, we recover the classical definition of the multivariate hypergeometric distribution.

If :math:`C > c`, then :math:`\sum_{i = 1}^d x_i \leq N`.
It allows one to recover the :class:`~openturns.Hypergeometric` distribution as a special case when :math:`d = 1`.

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X_i} & = & N \frac{c_i}{C}, \quad i = 1, \ldots, d \\
        \Var{X_i} & = & N \frac{c_i}{C} \left(1 - \frac{c_i}{C}\right) \frac{C - N}{C - 1}, \quad i = 1, \ldots, d \\
        \Cov{X_i, X_j} & = & - N \frac{c_i c_j}{C^2} \frac{C - N}{C - 1}, \quad i, j = 1, \ldots, d, i \neq j
    \end{eqnarray*}

The rectangular probabilities :math:`\Prob{\vect{X} \in B}`, i.e. the CDF and
the probability of an interval, are computed using the Poisson summation-based
generating function approach [lebrun2013]_. Their accuracy is controlled by two
parameters:

- *eta*, the maximum absolute error of the Poisson summation-based
  computation;

- *a_min*, the threshold below which the generating function of a truncated
  binomial distribution is computed with its polynomial form rather than as a
  correction of the untruncated generating function.

Those two parameters can be tuned through the :class:`~openturns.ResourceMap`
keys:

- ``MultivariateHypergeometric-eta``: Scalar, maximum absolute error of the
  Poisson summation-based CDF computation, default value :math:`1.0e-9`;
- ``MultivariateHypergeometric-smallA``: Scalar, threshold below which the
  generating function of a truncated binomial distribution is computed with
  its polynomial form, default value :math:`10.0`.

See Also
--------
openturns.Multinomial
openturns.Hypergeometric

Examples
--------
Create a distribution:

>>> import openturns.experimental as otexp
>>> distribution = otexp.MultivariateHypergeometric(3, [3, 4, 5])

Evaluate the PDF and the CDF at a point:

>>> distribution.computePDF([1, 1, 1])
0.272727...
>>> distribution.computeCDF([3, 4, 5])
1.0

The 1D case reduces to the :class:`~openturns.Hypergeometric` distribution:

>>> distribution1D = otexp.MultivariateHypergeometric(4, [3], 8)
>>> import openturns as ot
>>> hyper = ot.Hypergeometric(8, 3, 4)
>>> distribution1D.computePDF([2])
0.428571...

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::getN
"Accessor to the number of draws parameter :math:`N`.

Returns
-------
N : int
    The number of draws :math:`N`."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::setN
R"RAW(Accessor to the number of draws parameter :math:`N`.

Parameters
----------
N : int, :math:`0 \leq N \leq C`
    The number of draws :math:`N`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::getColors
R"RAW(Accessor to the population counts parameter :math:`\vect{c}`.

Returns
-------
colors : :class:`~openturns.Point`
    The population counts of each tracked category :math:`\vect{c}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::setColors
R"RAW(Accessor to the population counts parameter :math:`\vect{c}`.

Parameters
----------
colors : sequence of int, :math:`c_i \geq 0` and :math:`\sum_{i = 1}^d c_i \leq C`
    The population counts of each tracked category :math:`\vect{c}` (all nonnegative,
    with sum less or equal to the total population).)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::getTotal
R"RAW(Accessor to the total population size :math:`C`.

Returns
-------
total : int
    The total population size :math:`C`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::setTotal
R"RAW(Accessor to the total population size :math:`C`.

Parameters
----------
total : int, :math:`C \geq \max(N, \sum_{i = 1}^d c_i)`
    The total population size :math:`C`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::getEta
"Accessor to the maximum absolute error in CDF computation.

Returns
-------
eta : float
    The maximum absolute error in the Poisson summation-based CDF computation.

The default value comes from the ``MultivariateHypergeometric-eta``
:class:`~openturns.ResourceMap` key, equal to :math:`1.0e-9`."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::setEta
R"RAW(Accessor to the maximum absolute error in CDF computation.

Parameters
----------
eta : float, :math:`0 \leq eta < 1`
    The maximum absolute error in the Poisson summation-based CDF computation.

A value of 0 turns the error control off.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::getSmallA
"Accessor to the threshold in generating function computation.

Returns
-------
smallA : float
    The threshold triggering the algorithm for the computation of the generating
    function of a truncated binomial distribution. If the argument of the
    generating function is smaller than *smallA* the computation is done using
    the polynomial form of the generating function, otherwise it is done as a
    correction of the generating function of the untruncated binomial distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometric::setSmallA
R"RAW(Accessor to the threshold in generating function computation.

Parameters
----------
smallA : float, :math:`smallA \geq 0`
    The threshold triggering the algorithm for the computation of the generating
    function of a truncated binomial distribution. If the argument of the
    generating function is smaller than *smallA* the computation is done using
    the polynomial form of the generating function, otherwise it is done as a
    correction of the generating function of the untruncated binomial distribution.

The default value comes from the ``MultivariateHypergeometric-smallA``
:class:`~openturns.ResourceMap` key, equal to :math:`10.0`.)RAW"