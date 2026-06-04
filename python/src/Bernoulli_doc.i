%feature("docstring") OT::Bernoulli
R"RAW(Bernoulli distribution.

Its probability distribution function is defined as:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Prob{X = 1} & = & p \\
        \Prob{X = 0} & = & 1 - p
    \end{eqnarray*}

with :math:`0 \leq p \leq 1`.

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & p \\
        \Var{X} & = & p\,(1 - p)
    \end{eqnarray*}

Parameters
----------
p : float, :math:`0 \leq p \leq 1`, optional
    Success probability.

    Default value is 0.5.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.Bernoulli(0.2)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Bernoulli::getP
"Accessor to the distribution's *success probability* parameter.

Returns
-------
p : float
    Success probability."

// ---------------------------------------------------------------------

%feature("docstring") OT::Bernoulli::setP
R"RAW(Accessor to the distribution's *success probability* parameter.

Parameters
----------
p : float, :math:`0 \leq p \leq 1`
    Success probability.)RAW"
