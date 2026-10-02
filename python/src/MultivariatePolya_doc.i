%feature("docstring") OT::MultivariatePolya
R"RAW(Multivariate Polya distribution.

Randomizes drawing *N* balls from an urn initially split into *d* tracked categories
with concentrations :math:`(\alpha_1, \ldots, \alpha_d)` plus an implicit rest category:
after each draw the drawn ball is replaced together with *c* additional balls of the same
category.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
n : int, :math:`n \geq 1`
    The number of draws.

    Default value is 1.
alpha : sequence of float, :math:`\alpha_i > 0`, :math:`\sum_{i = 1}^d \alpha_i \leq Q`
    The concentration parameters of the tracked categories.

    Default value is [1].
c : float, :math:`c \geq 0`
    The reinforcement parameter.

    Default value is 1.
total : float, :math:`Q \geq \sum_{i = 1}^d \alpha_i`
    The total concentration.

    Default value is :math:`\sum_{i = 1}^d \alpha_i` (classical case with empty rest).

Notes
-----

This distribution differs from the classical Dirichlet-multinomial definition.
The classical definition is constrained by :math:`Q = \sum_{i = 1}^d \alpha_i` and is supported by
:math:`\left\{\vect{x} \in \Nset^d\, |\, \sum_{i = 1}^d x_i = N\right\}`. The OpenTURNS distribution is
constrained by :math:`Q \geq \sum_{i = 1}^d \alpha_i` and is supported in general by
:math:`\cS=\left\{\vect{x} \in \Nset^d\, |\, \sum_{i = 1}^d x_i \leq N\right\}`.

Let :math:`Q` be the total concentration, and :math:`\mathrm{Po}(x, m; c)` be a
generalized rising factorial defined by

.. math::

    \mathrm{Po}(x, m; c) = x (x + c) \cdots (x + (m - 1) c) = c^m \frac{\Gamma(x / c + m)}{\Gamma(x / c)}, \quad c > 0.

The probability distribution function of the number of observed balls of each
tracked category :math:`\vect{X} = (X_1, \ldots, X_d)` is defined by:

.. math::

    \Prob{\vect{X} = \vect{x}} = \frac{N!}{\mathrm{Po}(Q, N; c)} \left[\prod_{i = 1}^d \frac{\mathrm{Po}(\alpha_i, x_i; c)}{x_i!}\right] \frac{\mathrm{Po}(Q - q, N - s; c)}{(N - s)!}\mathbf{1}_{\vect{x} \in \cS}

where :math:`q = \sum_{i = 1}^d \alpha_i`, :math:`s = \sum_{i = 1}^d x_i`.
When :math:`c = 0` this PMF degenerates into
the :class:`~openturns.Multinomial` one with probabilities
:math:`p_i = \alpha_i / Q`, to which the parametric family also extends
continuously.

If :math:`Q = q`, then the rest is empty and :math:`\sum_{i = 1}^d x_i = N`.
In this case, we recover the classical definition of the Dirichlet-multinomial distribution.

If :math:`Q > q`, then :math:`\sum_{i = 1}^d x_i \leq N`.
It allows one to recover the beta-binomial distribution as a special case when :math:`d = 1`.

The first moments of the distribution are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X_i} & = & N \frac{\alpha_i}{Q}, \quad i = 1, \ldots, d \\
        \Var{X_i} & = & N \frac{\alpha_i}{Q} \left(1 - \frac{\alpha_i}{Q}\right) \frac{N + \theta}{1 + \theta}, \quad \theta = \frac{Q}{c}, \quad i = 1, \ldots, d \\
        \Cov{X_i, X_j} & = & - N \frac{\alpha_i \alpha_j}{Q^2} \frac{N + \theta}{1 + \theta}, \quad i, j = 1, \ldots, d, i \neq j
    \end{eqnarray*}

where the overdispersion factor :math:`(N + \theta) / (1 + \theta)` is equal
to 1 in the degenerate multinomial case :math:`c = 0`.

The marginal distribution of a category is a
`beta-binomial distribution <https://en.wikipedia.org/wiki/Beta-binomial_distribution>`_,
i.e. :math:`\mathrm{X_i} \sim \mathrm{BetaBinomial}(N, \alpha_i / c, (Q - \alpha_i) / c)`
for :math:`c > 0`, and degenerates into the
:math:`\mathrm{Binomial}(N, \alpha_i / Q)` one when :math:`c = 0`.
Its rectangular probabilities :math:`\Prob{\vect{X} \in B}`, i.e. the CDF
and the probability of an interval, are computed using the Poisson
summation-based generating function approach [lebrun2013]_. Their accuracy is
controlled by two parameters:

- *eta*, the maximum absolute error of the Poisson summation-based
  computation;

- *a_min*, the threshold below which the generating function of a truncated
  negative binomial distribution is computed with its polynomial form rather
  than as a correction of the untruncated generating function.

Those two parameters can be tuned through the :class:`~openturns.ResourceMap`
keys:

- ``MultivariatePolya-eta``: Scalar, maximum absolute error of the
  Poisson summation-based CDF computation, default value :math:`1.0e-9`;
- ``MultivariatePolya-smallA``: Scalar, threshold below which the
  generating function of a truncated negative binomial distribution is
  computed with its polynomial form, default value :math:`10.0`.

Sampling is based on the Dirichlet-compound representation of the distribution,
see the :class:`~openturns.Gamma` distribution.

See Also
--------
openturns.Multinomial

Examples
--------
Create a distribution:

>>> import openturns.experimental as otexp
>>> distribution = otexp.MultivariatePolya(6, [3, 4, 2], 1)

Evaluate the PDF and the CDF at a point:

>>> distribution.computePDF([2, 2, 2])
0.05994...
>>> distribution.computeCDF([6, 6, 6])
1.0

The 1D defective case reduces to the beta-binomial distribution:

>>> distribution1D = otexp.MultivariatePolya(5, [3], 2, 10)
>>> distribution1D.computePDF([2])
0.2148...

Draw a sample:

>>> sample = distribution.getSample(5)

Draw the PDF of a two-dimensional distribution:

>>> distribution2D = otexp.MultivariatePolya(6, [3, 4], 1)
>>> graph = distribution2D.drawPDF()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::getN
"Accessor to the number of draws parameter :math:`N`.

Returns
-------
N : int
    The number of draws :math:`N`."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::setN
R"RAW(Accessor to the number of draws parameter :math:`N`.

Parameters
----------
N : int, :math:`N \geq 1`
    The number of draws :math:`N`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::getAlpha
R"RAW(Accessor to the concentration parameters :math:`\vect{\alpha}`.

Returns
-------
alpha : :class:`~openturns.Point`
    The concentration parameters of each category :math:`\vect{\alpha}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::setAlpha
R"RAW(Accessor to the concentration parameters :math:`\vect{\alpha}`.

Parameters
----------
alpha : sequence of float, :math:`\alpha_i > 0`, :math:`\sum_{i = 1}^d \alpha_i \leq Q`
    The concentration parameters of each tracked category :math:`\vect{\alpha}`
    (all strictly positive, with sum less or equal to the total).)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::getTotal
R"RAW(Accessor to the total concentration :math:`Q`.

Returns
-------
total : float
    The total concentration :math:`Q`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::setTotal
R"RAW(Accessor to the total concentration :math:`Q`.

Parameters
----------
total : float, :math:`Q \geq \sum_{i = 1}^d \alpha_i`
    The total concentration :math:`Q`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::getC
"Accessor to the reinforcement parameter :math:`c`.

Returns
-------
c : float
    The reinforcement parameter :math:`c`."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::setC
R"RAW(Accessor to the reinforcement parameter :math:`c`.

Parameters
----------
c : float, :math:`c \geq 0`
    The reinforcement parameter :math:`c`.

A value equal to 0 gives the degenerate multinomial member of the family.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::getEta
"Accessor to the maximum absolute error in CDF computation.

Returns
-------
eta : float
    The maximum absolute error in the Poisson summation-based CDF computation.

The default value comes from the ``MultivariatePolya-eta``
:class:`~openturns.ResourceMap` key, equal to :math:`1.0e-9`."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::setEta
R"RAW(Accessor to the maximum absolute error in CDF computation.

Parameters
----------
eta : float, :math:`0 \leq eta < 1`
    The maximum absolute error in the Poisson summation-based CDF computation.

A value of 0 turns the error control off.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::getSmallA
"Accessor to the threshold in generating function computation.

Returns
-------
smallA : float
    The threshold triggering the algorithm for the computation of the generating
    function of a truncated negative binomial distribution. If the argument of
    the generating function is smaller than *smallA* the computation is done
    using the polynomial form of the generating function, otherwise it is done
    as a correction of the generating function of the untruncated negative
    binomial distribution."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolya::setSmallA
R"RAW(Accessor to the threshold in generating function computation.

Parameters
----------
smallA : float, :math:`smallA \geq 0`
    The threshold triggering the algorithm for the computation of the generating
    function of a truncated negative binomial distribution. If the argument of
    the generating function is smaller than *smallA* the computation is done
    using the polynomial form of the generating function, otherwise it is done
    as a correction of the generating function of the untruncated negative
    binomial distribution.

The default value comes from the ``MultivariatePolya-smallA``
:class:`~openturns.ResourceMap` key, equal to :math:`10.0`.)RAW"