%feature("docstring") OT::SpecFunc::BesselI0
R"RAW(Modified first kind Bessel function of order 0.

.. math::

    \forall x \in \Rset, \quad
    \mathrm{I}_0(x) = \sum_{m=0}^\infty\frac{1}{m!^2}\left(\frac{x}{2}\right)^{2m}

Parameters
----------
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::LogBesselI0
R"RAW(Logarithm of the modified first kind Bessel function of order 0.

.. math::

    \forall x \in \Rset, \quad
    LogBesselI0(x) = \log (\mathrm{I}_0(x))

See also
--------
openturns.SpecFunc.BesselI0

Parameters
----------
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::BesselI1
R"RAW(Modified first kind Bessel function of order 1.

.. math::

    \forall x \in \Rset, \quad
    \mathrm{I}_1(x) = \sum_{m=0}^\infty\frac{1}{m!(m+1)!}\left(\frac{x}{2}\right)^{2m+1}

Parameters
----------
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::LogBesselI1
R"RAW(Logarithm of the modified first kind Bessel function of order 1.

.. math::

    \forall x \in \Rset, \quad
    LogBesselI1(x) = \log (\mathrm{I}_1(x))

See also
--------
openturns.SpecFunc.BesselI1

Parameters
----------
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::BesselK
R"RAW(Modified second kind Bessel function of order :math:`\nu`.

.. math::

    \forall x \in \Rset, \quad
    \mathrm{K}_{\nu}(x) = \frac{\pi}{2}\frac{\mathrm{I}_{-\nu}(x)-\mathrm{I}_{\nu}(x)}{\sin{\nu\pi}}

Parameters
----------
nu : float
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::BesselKDerivative
R"RAW(Derivative of the modified second kind Bessel function of order :math:`\nu`.

.. math::

    \forall x \in \Rset, \quad
    \mathrm{K'}_{\nu}(x) = \frac{\mathrm{K}_{\nu-1}(x) + \mathrm{K}_{\nu+1}(x)}{-2}

Parameters
----------
nu : float
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::LogBesselK
R"RAW(Logarithm of the modified second kind Bessel function of order :math:`\nu`.

.. math::

    \forall x \in \Rset, \quad
    LogBesselK(\nu, x) = \log(\mathrm{K}_{\nu}(x))

See also
--------
openturns.SpecFunc.BesselK

Parameters
----------
nu : float
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::DeltaLogBesselI10
R"RAW(Difference between the logarithms of Bessel I1 and I0.

.. math::

    \forall x \in \Rset, \quad
    \mathrm{DeltaLogBesselI10}(x) = \log(\mathrm{I}_1(x)) - \log(\mathrm{I}_0(x))

See also
--------
openturns.SpecFunc.BesselI0
openturns.SpecFunc.BesselI1

Parameters
----------
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Beta
R"RAW(Beta function :math:`\mathrm{B}`.

.. math::

    \forall (a, b) > 0, \quad
    \mathrm{B}(a, b) = \int_0^1 t^{a-1}(1-t)^{b-1}\di{t}

Parameters
----------
a, b : float :math:`\in \Rset^*_+`

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::LogBeta
R"RAW(Logarithm of the Beta function.

.. math::

    \forall (a, b) > 0, \quad
    LogBeta(a, b) = \log (\mathrm{B}(a, b))

See also
--------
openturns.SpecFunc.Beta

Parameters
----------
a, b : float :math:`\in \Rset^*_+`

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::IncompleteBeta
R"RAW(Incomplete Beta function.

.. math::

    \forall (a, b) > 0, t \in [0, 1], \quad
    \mathrm{B}(x; a, b) = \int_0^x t^{a-1}(1-t)^{b-1}\di{t}

Parameters
----------
a, b : float :math:`\in \Rset^*_+`
x : float
tail : bool, optional
    By default, *tail* is *False*.

Returns
-------
result : float
    - If *tail* is *False*: :math:`result = \mathrm{B}(x; a, b)`.
    - If *tail* is *True*: :math:`result = \mathrm{B}(a, b) - \mathrm{B}(x; a, b)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::IncompleteBetaInverse
R"RAW(Inverse of the incomplete Beta function.

.. math::

    \forall (a, b) > 0 \quad
    IncompleteBetaInverse(x; a, b) = \mathrm{B}^{-1}(x/\mathrm{B}(a, b); a, b)

See also
--------
openturns.SpecFunc.IncompleteBeta
openturns.SpecFunc.RegularizedIncompleteBetaInverse

Parameters
----------
a, b : float :math:`\in \Rset^*_+`
x : float
tail : bool, optional
    By default, *tail* is *False*.

Returns
-------
result : float
    - If *tail* is *False*: :math:`result = \mathrm{B}^{-1}(x/\mathrm{B}(a, b); a, b)`.
    - If *tail* is *True*: :math:`result = 1 - \mathrm{B}^{-1}(x/\mathrm{B}(a, b); b, a)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::RegularizedIncompleteBeta
R"RAW(Regularized incomplete Beta function.

.. math::

    \forall (a, b) > 0 \quad
    \mathrm{I}(x; a, b) = \frac{\mathrm{B}(x; a, b)}{\mathrm{B}(a, b)}
                        = \frac{1}{\mathrm{B}(a, b)} \int_0^x t^{a-1}(1-t)^{b-1}\di{t}

with :math:`B(a, b)` the Beta function and :math:`B(x; a, b)` the incomplete
Beta function.

See also
--------
openturns.SpecFunc.IncompleteBeta
openturns.SpecFunc.Beta

Parameters
----------
a, b : float :math:`\in \Rset^*_+`
x : float
tail : bool, optional
    By default, *tail* is *False*.

Returns
-------
result : float
    - If *tail* is *False*: :math:`result = \mathrm{I}(x; a, b)`.
    - If *tail* is *True*: :math:`result = 1 - \mathrm{I}(x; a, b)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::RegularizedIncompleteBetaInverse
R"RAW(Inverse of the regularized incomplete Beta function.

.. math::

    \forall (a, b) > 0, \quad
    RegularizedIncompleteBetaInverse(x; a, b) = \mathrm{I}^{-1}(x; a, b)

See also
--------
openturns.SpecFunc.RegularizedIncompleteBeta

Parameters
----------
a, b : float :math:`\in \Rset^*_+`
x : float
tail : bool, optional
    By default, *tail* is *False*.

Returns
-------
result : float
    - If *tail* is *False*: :math:`result = \mathrm{I}^{-1}(x; a, b)`.
    - If *tail* is *True*: :math:`result = 1 - \mathrm{I}^{-1}(x; b, a)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Dawson
R"RAW(Dawson function.

.. math::

    \forall x \in \Cset, \quad
    \mathrm{D}_+(x) = \exp(-x^2)\int_0^x \exp(t^2)\di{t}

Parameters
----------
x : float or complex

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Debye
R"RAW(Debye function of order :math:`n`.

.. math::

    \forall x \in \Rset, \forall n \in \Nset^* \text{and} \, n \leq 20, \quad
    \mathrm{D}_n(x) = \frac{n}{x^n} \int_0^x \frac{t^n}{\exp(t)-1}\di{t}

Parameters
----------
x : float
n : int :math:`\in \{1, \cdots, 20\}`

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Ei
R"RAW(Exponential integral function.

.. math::

    \forall z \in \Cset, \quad
    \mathrm{Ei}(z) = -\int_{-z}^{\infty} \frac{\exp(-t)}{t}\di{t}

Parameters
----------
z : float or complex

Returns
-------
result : float or complex (same as z)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Factorial
R"RAW(Factorial function.

.. math::

    \forall n \in \Nset, \quad
    \mathrm{Factorial}(n) = n!

See also
--------
openturns.SpecFunc.LogFactorial

Parameters
----------
n : int

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Faddeeva
R"RAW(Complex Faddeeva function.

.. math::

    \forall x \in \Cset, \quad
    \mathrm{W}(x) = \exp(-x^2)\mathrm{erfc}(-ix)

with :math:`ErfC` the complementary error function.

See also
--------
openturns.SpecFunc.ErfC

Parameters
----------
x : float or complex

Returns
-------
result : complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::FaddeevaIm
R"RAW(Imaginary part of the Faddeeva function.

.. math::

    \forall x \in \Rset, \quad
    FaddeevaIm(x) = \Im (\mathrm{W}(x))

See also
--------
openturns.SpecFunc.Faddeeva

Parameters
----------
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Gamma
R"RAW(Gamma function :math:`\Gamma`.

.. math::

    \forall a \in \Cset, \quad
    \Gamma(a) = \int_0^{\infty} t^{a-1}\exp(-t)\di{t}

Parameters
----------
a : float or complex

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::LogGamma
R"RAW(Logarithm of the Gamma function.

.. math::

    \forall a \in \Cset, \quad
    LogGamma(a) = \log (\Gamma(a))

See also
--------
openturns.SpecFunc.Gamma

Parameters
----------
a : float or complex

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::LogGamma1p
R"RAW(LogGamma1p function.

.. math::

    \forall a \in \Rset, \quad
    LogGamma1p(a) = \log (\Gamma(1+a))

with :math:`\Gamma` the Gamma function.

See also
--------
openturns.SpecFunc.Gamma

Parameters
----------
a : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::IncompleteGamma
R"RAW(Incomplete Gamma function.

.. math::

    \forall x \in \Rset, \quad
    \gamma(a, x) = \int_0^x t^{a-1}\exp(-t)\di{t}

Parameters
----------
a : float :math:`\in \Rset^*_+`
x : float
tail : bool, optional
    By default, *tail* is *False*.

Returns
-------
result : float
    - If *tail* is *False*: :math:`result = \gamma(a, x)`.
    - If *tail* is *True*: :math:`result = \Gamma(a) - \gamma(a, x)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::IncompleteGammaInverse
R"RAW(Inverse of the incomplete Gamma function with respect to :math:`x`.

.. math::

    IncompleteGammaInverse(a, x) = \gamma^{-1}(a, x)

See also
--------
openturns.SpecFunc.IncompleteGamma

Parameters
----------
a : float :math:`\in \Rset^*_+`
x : float
tail : bool, optional
    By default, *tail* is *False*.

Returns
-------
result : float
    - If *tail* is *False*: :math:`result = \mathrm{P}^{-1}(a, x/\Gamma(a))`.
    - If *tail* is *True*: :math:`result = \mathrm{P}^{-1}(a, (1-x)/\Gamma(a))`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::RegularizedIncompleteGamma
R"RAW(Regularized incomplete Gamma function.

.. math::

    \forall x \in \Rset, \quad
    \mathrm{P}(a, x) = \frac{\gamma(a, x)}{\Gamma(a)}
                     = \frac{1}{\Gamma(a)}\int_0^x t^{a-1}\exp(-t)\di{t}

See also
--------
openturns.SpecFunc.Gamma
openturns.SpecFunc.IncompleteGamma

Parameters
----------
a : float :math:`\in \Rset^*_+`
x : float
tail : bool, optional
    By default, *tail* is *False*.

Returns
-------
result : float
    - If *tail* is *False*: :math:`result = \mathrm{P}(a, x)`.
    - If *tail* is *True*: :math:`result = \Gamma(a) - \mathrm{P}(a, x)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::RegularizedIncompleteGammaInverse
R"RAW(Inverse of the regularized incomplete Gamma function.

.. math::

    \forall x \in \Rset, \quad
    RegularizedIncompleteGammaInverse(a, x) = \mathrm{P}^{-1}(a, x)

See also
--------
openturns.SpecFunc.Gamma
openturns.SpecFunc.RegularizedIncompleteGamma

Parameters
----------
a : float :math:`\in \Rset^*_+`
x : float :math:`\in [0, 1]`
tail : bool, optional
    By default, *tail* is *False*.

Returns
-------
result : float
    - If *tail* is *False*: :math:`result = \mathrm{P}^{-1}(a, x)`.
    - If *tail* is *True*: :math:`result = \mathrm{P}^{-1}(a, 1-x)`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::DiGamma
R"RAW(Digamma function.

.. math::

    \Psi(x) = \frac{1}{\Gamma(x)}\frac{\mathrm{d} \Gamma(x)}{\mathrm{d}x}

with :math:`\Gamma` the Gamma function.

See also
--------
openturns.SpecFunc.Gamma

Parameters
----------
x : float :math:`\in \Rset^*_+` or complex :math:`\in \Cset \setminus \Rset_-`

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Psi
R"RAW(Psi function, alias for digamma function.

.. math::

    \Psi(x) = \frac{1}{\Gamma(x)}\frac{\mathrm{d} \Gamma(x)}{\mathrm{d}x}

with :math:`\Gamma` the Gamma function.

See also
--------
openturns.SpecFunc.Gamma
openturns.SpecFunc.DiGamma

Parameters
----------
x : float :math:`\in \Rset^*_+` or complex :math:`\in \Cset \setminus \Rset_-`

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::DiGammaInv
R"RAW(Inverse of the DiGamma function.

.. math::

    DiGammaInv(x) = \Psi^{-1} (x)

See also
--------
openturns.SpecFunc.DiGamma

Parameters
----------
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::TriGamma
R"RAW(TriGamma function.

.. math::

    \Psi_1(x) = \frac{1}{\Gamma(x)}\frac{\mathrm{d}^2 \Gamma(x)}{\mathrm{d}x^2}

with :math:`\Gamma` the Gamma function.

See also
--------
openturns.SpecFunc.Gamma

Parameters
----------
x : float :math:`\in \Rset^*_+`

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::IGamma1pm1
R"RAW(IGamma1pm1 function.

.. math::

    \forall x \in \Rset, \quad
    IGamma1pm1(a, x) = \int_0^x t^{a-1}\exp(-t)\di{t}

Parameters
----------
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::GammaCorrection
R"RAW(GammaCorrection function.

.. math::

    \forall x \in \Rset^*_+, \quad
    GammaCorrection(a) = \log (\Gamma(a)) - \log (\sqrt{2\Pi}) + a - (a - 0.5) \log(a)

with :math:`\Gamma` the Gamma function.

See also
--------
openturns.SpecFunc.Gamma

Parameters
----------
a : float :math:`\in \Rset^*_+`

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::HyperGeom_1_1
R"RAW(Hypergeometric function of type (1,1).

.. math::

    {}_1F_1(p_1, q_1, x) = \sum_{n=0}^{\infty}
                           \left[
                           \prod_{k=0}^{n-1} \frac{(p_1 + k)}{(q_1 + k)}
                           \right]  \frac{x^n}{n!}

Parameters
----------
p1, q1 : float
x : float or complex

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::HyperGeom_2_1
R"RAW(Hypergeometric function of type (2,1).

.. math::

    {}_2F_1(p_1, p_2, q_1, x) = \sum_{n=0}^{\infty}
                                \left[
                                \prod_{k=0}^{n-1} \frac{(p_1 + k)(p_2 + k)}{(q_1 + k)}
                                \right] \frac{x^n}{n!}

Parameters
----------
p1, p2, q1, x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::HyperGeom_2_2
R"RAW(Hypergeometric function of type (2,2).

.. math::

    {}_2F_2(p_1, p_2, q_1, q_2, x) = \sum_{n=0}^{\infty}
                         \left[
                         \prod_{k=0}^{n-1} \frac{(p_1 + k)(p_2 + k)}{(q_1 + k) (q_2 + k)}
                         \right] \frac{x^n}{n!}

Parameters
----------
p1, p2, q1, q2, x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Erf
R"RAW(Error function Erf.

.. math::

    \forall x \in \Cset, \quad
    Erf(x) = \frac{2}{\sqrt{\pi}} \int_0^x \exp(-t^2)\di{t}

Parameters
----------
x : float or complex

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::ErfC
R"RAW(Complementary error function ErfC.

.. math::

    \forall x \in \Cset, \quad
    ErfC(x) = 1 - Erf(x)

with :math:`Erf` the error function.

See also
--------
openturns.SpecFunc.Erf

Parameters
----------
x : float or complex

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::ErfInverse
R"RAW(Inverse of the error function Erf.

.. math::

    \forall x \in \Cset, \quad
    ErfInverse(x) = Erf^{-1} (x)

See also
--------
openturns.SpecFunc.Erf

Parameters
----------
x : float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::ErfCX
R"RAW(ErfCX function.

.. math::

    \forall x \in \Cset, \quad
    ErfCX(x) = \exp(x^2).ErfC(x)

with :math:`ErfC` the complementary error function.

See also
--------
openturns.SpecFunc.ErfC

Parameters
----------
x : float or complex

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::ErfI
R"RAW(Imaginary error function ErfI.

.. math::

    \forall x \in \Cset, \quad
    ErfI(x) = -i Erf(ix)

with :math:`Erf` the error function.

See also
--------
openturns.SpecFunc.Erf

Parameters
----------
x : float or complex

Returns
-------
result : float or complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Log1MExp
R"RAW(Log1MExp function.

.. math::

    \forall x \in \Rset^+, \quad
    Log1MExp(x) = \log (1-\exp(-x))

Parameters
----------
x : float :math:`\in \Rset^*_+`

Returns
-------
result : complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Expm1
R"RAW(Expm1 function.

.. math::

    \forall x \in \Cset, \quad
    Expm1(x) = \exp(x)-1

Parameters
----------
x : float or complex

Returns
-------
result : complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Log1p
R"RAW(Log1p function.

.. math::

    \forall x \in \Cset, \quad
    Log1p(x) = \log (1+x)

Parameters
----------
x : float or complex

Returns
-------
result : complex)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::DiLog
R"RAW(Dilogarithm function.

.. math::

    \forall x \in ]-\infty, 1[, \quad
    Li_2(x) = -\int_0^x \frac{\log (1-t)}{t}\di{t}

Parameters
----------
x : float :math:`\in ]-\infty, 1[`

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::IPow
"Raise the given :math:`x` to the integral power :math:`n`.

.. math::

    IPow(m, n) = m^n
    IPow(x, n) = x^n

Parameters
----------
m : int
n : int
x : float

Returns
-------
result : int or float
    The result has the same type as the first argument.

Examples
--------
>>> import openturns as ot
>>> ot.SpecFunc.IPow(-2.5, 3)
-15.625
>>> ot.SpecFunc.IPow(5, 3)
125"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::IRoot
R"RAW(Extract the :math:`n` integral root of the given :math:`x`.

.. math::

    IRoot(x, n) = \sqrt[n]{x}

Parameters
----------
n : int
x : float

Returns
-------
result : float

Examples
--------
>>> import openturns as ot
>>> ot.SpecFunc.IRoot(-15.625, 3)
-2.5)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::NextPowerOfTwo
R"RAW(Smallest power of two greater or equal to the given :math:`n`.

.. math::

    NextPowerOfTwo(n) = 2^{\lceil \log_2(n)\rceil}

Parameters
----------
n : positive int

Returns
-------
result : positive int

Examples
--------
>>> import openturns as ot
>>> int(ot.SpecFunc.NextPowerOfTwo(42))
64)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Log2
R"RAW(Integer base 2 logarithm of :math:`n`.

.. math::

    Log2(n) = \log_2(n)

Parameters
----------
n : positive int

Returns
-------
result : positive int

Examples
--------
>>> import openturns as ot
>>> int(ot.SpecFunc.Log2(42))
5)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::BitCount
"Compute the number of bits set to 1 in an integer.

Parameters
----------
n : positive int

Returns
-------
result : positive int

Examples
--------
>>> import openturns as ot
>>> int(ot.SpecFunc.BitCount(42))
3"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::LogBinomialCoefficient
R"RAW(Compute the natural logarithm of the binomial coefficient.

.. math::

    LogBinomialCoefficient(n, k) = \log\left( \frac{n!}{k! (n-k)!} \right)

Parameters
----------
n : nonegative int
k : nonegative int

Returns
-------
result : float

Examples
--------
>>> import openturns as ot
>>> ot.SpecFunc.LogBinomialCoefficient(10, 5)
5.5294...)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::LambertW
R"RAW(Lambert W function.

The Lambert W function :math:`\mathrm{W}(x)` is defined by the relation:

.. math::

    x = \mathrm{W}(x) \exp(\mathrm{W}(x))

Parameters
----------
x : float
principal : bool, optional
    By default, *principal* is *True*.

Returns
-------
result : float
    - If *principal* is *True* : :math:`result = \mathrm{W}_0(x)`.
      :math:`\mathrm{W}_0(x)` is referred to as the principal branch of the Lambert W
      function. It denotes the upper part of the function whose domain is
      :math:`[-1/e, +\infty[` and range :math:`[-1, +\infty[`.
    - If *principal* is *False* : :math:`result = \mathrm{W}_{-1}(x)`.
      :math:`\mathrm{W}_{-1}(x)` is the second real branch of the Lambert W function.
      It denotes the lower part of the function whose domain is
      :math:`[-1/e, 0[` and range :math:`]-\infty, -1]`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::LogFactorial
R"RAW(Logarithm of the factorial function.

.. math::

    \forall n \in \Nset, \quad
    LogFactorial(n) = \log(n!)

See also
--------
openturns.SpecFunc.Stirlerr

Parameters
----------
n : int

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Stirlerr
R"RAW(Error of the Stirling approximation of the factorial logarithm.

.. math::

    \forall n \in \Nset, \quad
    Stirlerr(n) = \log(n!) - \log\left(sqrt{2\pi n}\left(\dfrac{n}{e}\right)^n\right)

See also
--------
openturns.SpecFunc.LogFactorial

Parameters
----------
n : int

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::AccurateSum
R"RAW(Accurate summation.

Returns the value :math:`\sum_{i=0}^{n-1}x_i` using extended precision (if
available) or Kahan's method

Parameters
----------
x : sequence of float

Returns
-------
result : float)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SpecFunc::Clip01
"Clip a value on [0, 1].

This function can be useful when we want to ensure that a
given number can be considered as a probability, even if
there are e.g. rounding errors.

Parameters
----------
p : float
    Value to clip
tail : bool, default=False
    If False, then return min(1, max(0,p)). Otherwise return 1 - min(1, max(0,p)). 

Returns
-------
clip : float
    Clipped value.

Examples
--------
>>> import openturns as ot
>>> p = ot.SpecFunc.Clip01(1.01)"
