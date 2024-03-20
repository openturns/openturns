"""
=============================
Create multivariate functions
=============================
We can create multivariate functions by different methods. One of the methods is to gather
multivariate functions. The other one is to create a function having multivariate input using
the tensor product of univariate functions. In this example, we present both methods.
"""
# %%
# Description
# ===========
#
# We want to build some multivariate functions :math:`f : \mathbb{R}^d \mapsto \mathbb{R}^q`.
#
# We can do that:
#
# - Case 1: using a finite collection of multivariate functions :math:`f_i:  \mathbb{R}^d \mapsto \mathbb{R}^q`,
# - Case 2: using the tensor product of univariate functions,
# - Case 3: using the tensor product of orthogonal univariate polynomials,
# - Case 4: using the tensor product of orthogonal univariate functions.
#
#
# Case 1: Finite collection of multivariate functions
# ===================================================
#
# In that case, we have a  finite collection of multivariate functions
# :math:`f_i : \mathbb{R}^d \mapsto \mathbb{R}^q`.
# We use the :class:`~openturns.Basis` class.
#
# For example, we consider:
#
# .. math::
#     \begin{array}{lcl}
#       f_1(x_1, x_2) & = & (x_1^2, x_2^2)\\
#       f_2(x_1, x_2) & = & (x_1+1, 2x_2)\\
#       f_3(x_1, x_2) & = & (\cos(x_1x_2), x_2^3)
#     \end{array}

import openturns as ot

f1 = ot.SymbolicFunction(["x1", "x2"], ["x1^2", "x2^2"])
f2 = ot.SymbolicFunction(["x1", "x2"], ["x1+1.0", "2.0*x2"])
f3 = ot.SymbolicFunction(["x1", "x2"], ["cos(x1*x2)", "x2^3"])

myBasis = ot.Basis([f1, f2, f3])
f = myBasis.build(1)


# %%
# Case 2: Tensor product of univariate functions
# ==============================================
#
# In that case, the univariate functions are not necessarily orthogonal with respect to a measure.
#
# We have the family of functions: :math:`(x_i \mapsto \phi^i_k(x_i))_{k \geq 0}`
# for :math:`1 \leq i \leq d`.. The :meth:`enumerate` function of the :class:`~openturns.LinearEnumerateFunction`
# class stores the way the
# multivariate basis is enumerated: *enumerate(n)* is a multi-index
# :math:`\boldsymbol{\alpha} = (\alpha_1, \dots, \alpha_d)` such that the function number :math:`n` is:
#
# .. math::
#     f_n(\vect{x}) = \prod_{i=1}^d \phi^i_{\alpha_i}(x_i).
#
# We use the :class:`~openturns.TensorizedUniVariateFunctionFactory` class.
#
# For example, we consider: :math:`f: \mathbb{R}^2 \mapsto \mathbb{R}`, and:
#
# - dimension :math:`x_1`: the family of monomials: :math:`(x_1 \mapsto x_1^k)_{k \geq 0}`,
# - dimension :math:`x_2`: the family of Haar wavelets:  :math:`(x_2 \mapsto \phi^2_k(x_2))_{k \geq 0}`.

univFuncFamily_Mon = ot.MonomialFunctionFactory()
univFuncFamily_UnivPol = ot.OrthogonalUniVariatePolynomialFunctionFactory(
    ot.JacobiFactory()
)
univFuncFamily_Haar = ot.HaarWaveletFactory()
univFuncFamily_Fourier = ot.FourierSeriesFactory()

familyColl = [
    univFuncFamily_Mon,
    univFuncFamily_UnivPol,
    univFuncFamily_Haar,
    univFuncFamily_Fourier,
]
enumerateFunction = ot.LinearEnumerateFunction(len(familyColl))

familyFunction = ot.TensorizedUniVariateFunctionFactory(familyColl, enumerateFunction)
k = 3
f = familyFunction.build(k)
print("input dimension = ", f.getInputDimension())
print("output dimension = ", f.getOutputDimension())

# %%
# If we want to use an orthogonal univariate polynomials family, then we have to cast the family
# in the  :class:`~openturns.OrthogonalUniVariatePolynomialFunctionFactory` class.
#
# For example, we use the  Jacobi orthogonal univariate polynomials family.
univFuncFamily_Jacobi = ot.OrthogonalUniVariatePolynomialFunctionFactory(
    ot.JacobiFactory()
)


# %%
# Case 3:  Tensor product of orthogonal univariate polynomials
# ============================================================
#
# In that case, the univariate polynomials are orthogonal with respect to a measure :math:`\mu`.
#
# We have the family of polynomials: :math:`(x_i \mapsto \phi^i_k(x_i))_{k \geq 0}`
# for :math:`1 \leq i \leq d`. The :meth:`enumerate` function stores of the
# :class:`~openturns.LinearEnumerateFunction` class the way
# the multivariate basis is enumerated: *enumerate(n)* is a multi-index
# :math:`(\ell_1, \dots, \ell_d)` such that the function number :math:`n`
# is:
#
# .. math::
#     f_n(\vect{x}) = \prod_{i=1}^d \phi^i_{\ell_i}(x_i).
#
# We use the  :class:`~openturns.OrthogonalProductPolynomialFactory` class.
#
# For example, we consider : :math:`f: \mathbb{R}^2 \mapsto \mathbb{R}`, and:
#
# - dimension of :math:`x_1`: the family of Jacobi polynomials: :math:`(x_1 \mapsto x_1^k){k \geq 0}`,
# - dimension of :math:`x_2`: the family of Hermite polynomials: :math:`(x_2 \mapsto \phi^2_k(x_2))_{k \geq 0}`.

univFuncFamily_Jacobi = ot.JacobiFactory()
univFuncFamily_Hermite = ot.HermiteFactory()
familyColl = [univFuncFamily_Jacobi, univFuncFamily_Hermite]
enumerateFunction = ot.LinearEnumerateFunction(len(familyColl))
familyFunction = ot.OrthogonalProductPolynomialFactory(familyColl, enumerateFunction)
f = familyFunction.build(3)
print("input dimension = ", f.getInputDimension())
print("output dimension = ", f.getOutputDimension())

# %%
# We get the measure:
measure_Jacobi = ot.JacobiFactory().getMeasure()
measure_Hermite = ot.HermiteFactory().getMeasure()
print("Measure orthogonal to Jacobi polynomials = ", measure_Jacobi)
print("Measure orthogonal to Hermite polynomials = ", measure_Hermite)


# %%
# Case 4: Tensor product of orthogonal univariate functions
# =========================================================
#
# In that case, the univariate functions are orthogonal with respect to a measure :math:`\mu`.
#
# We have the family of functions: :math:`(x_i \mapsto \phi^i_k(x_i))_{k \geq 0}`
# for :math:`1 \leq i \leq d`. The :math:`enumerate` function of the :class:`~openturns.LinearEnumerateFunction`
# class stores the way
# the multivariate basis is enumerated: *enumerate(n)* is a multi-index
# :math:`\vect{\alpha} = (\alpha_1, \dots, \alpha_d)` such that the function number *n* is:
#
# .. math::
#
#      f_n(\vect{x}) = \prod_{i=1}^d \phi^i_{\alpha_i}(x_i)
#
# We use the :class:`~openturns.OrthogonalProductFunctionFactory` class.


# %%
# For example, we consider : :math:`f: \mathbb{R}^2 \mapsto \mathbb{R}`, and:
#
# - dimension :math:`x_1`: the family of Haar wavelets: :math:`(x_1 \mapsto \phi^1_k(x_1))_{k \geq 0}`,
# - dimension :math:`x_2`: the family of Fourier series functions: :math:`(x_2 \mapsto \phi^2_k(x_2)){k \geq 0}`.

univFuncFamily_Haar = ot.HaarWaveletFactory()
univFuncFamily_Fourier = ot.FourierSeriesFactory()
familyColl = [univFuncFamily_Haar, univFuncFamily_Fourier]
enumerateFunction = ot.LinearEnumerateFunction(len(familyColl))

familyFunction = ot.OrthogonalProductFunctionFactory(familyColl, enumerateFunction)
k = 3
f = familyFunction.build(k)

# %%
# We get the measure:
measure_Haar = ot.HaarWaveletFactory().getMeasure()
measure_Fourier = ot.FourierSeriesFactory().getMeasure()
print("Measure orthogonal to Haar wavelets = ", measure_Haar)
print("Measure orthogonal to Fourier series = ", measure_Fourier)
