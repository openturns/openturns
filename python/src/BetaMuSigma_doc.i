%feature("docstring") OT::BetaMuSigma
R"RAW(Beta distribution parameters.

Parameters
----------
mu : float
    Mean.

    Default value is 0.5.
sigma : float
    Standard deviation :math:`\sigma > 0`.

    Default value is 0.223607.
a : float
    Lower bound.

    Default value is 0.0.
b : float, :math:`b > a`
    Upper bound.

    Default value is 1.0.

Notes
-----
The native parameters  :math:`(\alpha, \beta, a, b)` are defined as follows:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \alpha & = & \left(\dfrac{\mu-a}{b-a}\right) \left( \dfrac{(b-\mu)(\mu-a)}{\sigma^2}-1\right) \\
        \beta & = &  \left( \dfrac{b-\mu}{\mu-a}\right) \alpha
    \end{eqnarray*}

See also
--------
Beta

Examples
--------
Create the parameters  :math:`(\mu, \sigma, a, b)` of the Beta distribution:

>>> import openturns as ot
>>> parameters = ot.BetaMuSigma(0.2, 0.6, -1, 2)

Convert parameters into the native parameters :math:`(\alpha, \beta, a, b)`:

>>> print(parameters.evaluate())
[2,3,-1,2])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BetaMuSigma::isElliptical
R"RAW(Test whether the Beta distribution is elliptical.

Returns
-------
test : bool
    Answer.
    
Notes
-----
The Beta distribution parametrized by the given :math:`(\mu, \sigma, a, b)`
is elliptical if :math:`b = 2\mu - a`.)RAW"
