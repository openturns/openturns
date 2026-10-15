%feature("docstring") OT::NormalCopula
R"RAW(Normal copula.

The Normal copula in dimension :math:`\inputDim` is defined by its
cumulative distribution function:

.. math::

    C(u_1, \cdots, u_\inputDim) = \Phi_{\inputDim, \mat{R}}(\Phi^{-1}(u_1), \cdots, \Phi^{-1}(u_\inputDim))

where the shape matrix :math:`\mat{R}` is a correlation matrix and
:math:`\Phi_{\inputDim, \mat{R}}` is the cumulative distribution function of the
normal distribution with zero mean, unit marginal variances and correlation :math:`R`:

.. math::

    \Phi_{\inputDim, \mat{R}}(\vect{x}) = \int_{-\infty}^{x_1} \ldots
                                   \int_{-\infty}^{x_\inputDim}
                                   \frac{1}
                                        {{(2\pi\det{\mat{R}})}^{\frac{\inputDim}{2}}}
                                 \exp \left(-\frac{\Tr{\vect{u}}\mat{R}\vect{u}}{2} \right)\di{\vect{u}}


The shape matrix :math:`\mat{R}` can be computed from a Spearman correlation
matrix :math:`\left(\rho_{ij}^S\right)_{1 \leq i,j \leq \inputDim}` as well as a
Kendall concordance matrix :math:`\left( \tau_{ij}\right)_{1 \leq i,j \leq \inputDim}`
as follows:

- From the Spearman correlation matrix:

  .. math ::
      :label: SpearmanToShape

      \mat{R}_{ij} = 2 \sin \left( \frac{\pi}{6}\rho_{ij}^S \right)


- From the Kendall concordance matrix:

  .. math::
      :label: KendallToShape

      \mat{R}_{ij} = \sin \left( \frac{\pi}{2} \tau_{ij} \right)

The shape matrix is the Pearson correlation matrix of a multivariate distribution
only if the normal copula is associated to normal margin distributions.

Note that relation :eq:`KendallToShape` is universal among the elliptical copulas, to the contrary of the Spearman correlation.

Available constructor:
    NormalCopula(*n*)

    NormalCopula(*R*)

Parameters
----------
n : int, :math:`n \geq 1`
    Dimension of the copula.

    Default value is 2.

R : :class:`~openturns.CorrelationMatrix`
    Shape matrix :math:`\mat{R}`.

    Default value is the identity matrix.

See also
--------
Distribution

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> R = ot.CorrelationMatrix(3)
>>> R[0, 1] = 0.25
>>> R[1, 2] = 0.25
>>> copula = ot.NormalCopula(R)

Draw a sample:

>>> sample = copula.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::NormalCopula::GetCorrelationFromKendallCorrelation
R"RAW(Get the shape matrix from the Kendall correlation matrix.

Parameters
----------
K : :class:`~openturns.CorrelationMatrix`
    Kendall correlation matrix :math:`\left(\tau_{ij}\right)_{1 \leq i,j \leq \inputDim}`.

Returns
-------
R : :class:`~openturns.CorrelationMatrix`
    Shape matrix :math:`\mat{R}`.

Notes
-----
The shape matrix :math:`\mat{R}` is computed following :eq:`KendallToShape`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::NormalCopula::GetCorrelationFromSpearmanCorrelation
R"RAW(Get the shape matrix from the Spearman correlation matrix.

Parameters
----------
S : :class:`~openturns.CorrelationMatrix`
    Spearman correlation matrix :math:`\left(\rho_{ij}^S\right)_{1 \leq i,j \leq \inputDim}`.

Returns
-------
R : :class:`~openturns.CorrelationMatrix`
    Shape matrix :math:`\mat{R}`.

Notes
-----
The shape matrix :math:`\mat{R}` is computed following :eq:`KendallToShape`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::NormalCopula::getShapeMatrix
"Accessor to the shape matrix.

Returns
-------
R : :class:`~openturns.CorrelationMatrix`
    Shape matrix :math:`\mat{R}` given to the constructor. It generally
    differs from the Pearson correlation matrix returned by
    :meth:`getCorrelation` (they coincide for the identity matrix)."

