%feature("docstring") OT::Hypergeometric
R"RAW(Hypergeometric distribution.

Parameters
----------
n : int, :math:`n \in \Nset`
    Population size.

    Default value is 0.
k : int, :math:`0 \leq k \leq n`
    Number of candidates in the population.

    Default value is 1.
m : int, :math:`0 \leq m \leq n`
    Size of the sample drawn in the population.

    Default value is 0.

Notes
-----
Its probability distribution function is defined as:

.. math::

    \Prob{X = i} = \dfrac{{k \choose i}{n-m \choose m-i}}{{n \choose m}}
                   \quad \forall i \in \{\max\{0,k+m-n\}, \ldots, \min\{k,m\}\}

Its first moments are:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{X} & = & n\dfrac{k}{m} \\
        \Var{X} & = & m\dfrac{k}{n}\dfrac{n-k}{n}\dfrac{n-m}{n-1}
    \end{eqnarray*}

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> distribution = ot.Hypergeometric(10, 5, 4)

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Hypergeometric::getK
R"RAW(Accessor to the number of candidates parameter.

Returns
-------
k : int, :math:`0\leq k\leq n`
    The number of candidates in the population.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Hypergeometric::getM
R"RAW(Accessor to the sample size.

Returns
-------
m : int, :math:`0\leq m\leq n`
    The sample size.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Hypergeometric::getN
"Accessor to the population size.

Returns
-------
n : int
    The population size."

// ---------------------------------------------------------------------

%feature("docstring") OT::Hypergeometric::setK
R"RAW(Accessor to the number of candidates parameter.

Parameters
----------
k : int, :math:`0\leq k\leq n`
    The number of candidates in the population.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Hypergeometric::setM
R"RAW(Accessor to the sample size.

Parameters
----------
m : int, :math:`0\leq m\leq n`
    The sample size.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Hypergeometric::setN
"Accessor to the population size.

Parameters
----------
n : int
    The population size."

