%feature("docstring") OT::Polya
R"RAW(Polya distribution.


Parameters
----------
r : float, :math:`r > 0`
    Stopping time of the Bernoulli process.

    Default value is 1.0.

p : float, :math:`0 < p < 1`
    Success probability of the Bernoulli trial.

    Default value is 0.5.

Notes
-----
Its probability distribution function is defined as:

.. math::

    \Prob{X = k} = \frac{\Gamma(k + r)}{\Gamma(r)\Gamma(k+1)}p^k(1-p)^r,
                   \, \forall k \in \Nset

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & \frac{rp}{1-p} \\
        \Var{X} & = & \frac{rp}{(1-p)^2}
    \end{eqnarray*}

When :math:`r` is a positive integer, it is the number of failures
before the first success of the Bernoulli trial.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.Polya(1.0, 0.6)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Polya::getP
"Accessor to the success probability parameter.

Returns
-------
p : float
    The success probability of the Bernoulli trial."

// ---------------------------------------------------------------------

%feature("docstring") OT::Polya::getR
"Accessor to the stopping time parameter.

Returns
-------
r : float
    Stopping time of the Bernoulli process."

// ---------------------------------------------------------------------

%feature("docstring") OT::Polya::setP
R"RAW(Accessor to the success probability parameter.

Parameters
----------
p : float, :math:`0 \leq p \leq 1`
    The success probability of the Bernoulli trial.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Polya::setR
"Accessor to the stopping time parameter.

Parameters
----------
r : float, :math:`r > 0`
    Stopping time of the Bernoulli process."
