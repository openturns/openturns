.. _least_squares:

Least squares meta models
-------------------------

A least squares meta model provides an approximation of the model :math:`\model: \Rset^\inputDim \rightarrow \Rset^\outputDim` which is valid over its whole domain of definition.

Let :math:`\model: \Rset^\inputDim \rightarrow \Rset^\outputDim`. The objective is to create a meta model
:math:`\metaModel` defined by:

  .. math::
      :label: LSMetaModel
      \metaModel(\vect{x})  = \sum_{j=0}^N \vect{a}_j \Psi_j(\vect{x})

where, for :math:`0 \leq j \leq N`, :math:`\vect{a}_j \in \Rset^\outputDim` and
:math:`\psi_j: \Rset^\inputDim \rightarrow \Rset` with :math:`\psi_0: \vect{x} \rightarrow 1`.

The functional basis :math:`(\psi_j)_{0 \leq j \leq N}` is specified and the objective it to determine the
coefficients :math:`(\vect{a}_j)_{0 \leq j \leq N}`.

Let :math:`\mat{A} \in \cM_{N+1, \outputDim}` and :math:`\vect{\Psi(\vect{x})} \in \Rset^{N+1}` be defined by:

  .. math::
    :label: defAPsi
    \mat{A} & = \Tr{(\vect{a}_0 | \dots | \vect{a}_N)}\\
    \vect{\Psi(\vect{x})} & = \Tr{(\Psi_0(\vect{x}), \dots, \Psi_N(\vect{x})}

Then the meta model :eq:`LSMetaModel` can be written as:

  .. math::
      :label: LSMetaModelMat
      \vect{y}  =  \Tr{\mat{A}}\vect{\Psi(\vect{x})}

Let :math:`\cX` be an experimental design of size :math:`\sampleSize`, that is, a set of observations of
the input vector defined by:

.. math::

    \cX = \left\{ \vect{x}^{(1)}, \dots, \vect{x}^{(\sampleSize)} \right\},

as well as the corresponding ouput vectors:

.. math::

    \cY = \left\{ \vect{y}^{(1)}, \dots, \vect{y}^{(\sampleSize)} \right\}.

where :math:`\vect{y}^{(k)} = \model{ \vect{x}^{(k)}}`.

We define the objective function :math:`\cJ` by:

.. math::
    :label: objFunc
    \cJ(\mat{A}) = \sum_{i=1}^\sampleSize \left \| \vect{y}^{(i)} - \Tr{\mat{A}} \vect{\Psi(\vect{x}^{(i))}
    \right \|_{L^2}^2

Then we search :math:`\widehat{\mat{A}}` that minimizes the objective function :eq:`objFunc`:

.. math::
    :label: defA
    \widehat{\mat{A}} = \argmin \cJ(\mat{A})

Let the matrices :math:`\mat{Psi} \in \cM_{\sampleSize, N+1}` and :math:`\mat{Y}\in \cM_{\sampleSize, \outputDim}`
be defined by:

.. math::
    \mat{Psi} & = (\psi_j(\vect{x}^{(i)}))_{i,j}\\
    \mat{Y}    & = \Tr{(\vect{y}^{(1)} | \dots | \vect{y}^{(\sampleSize)}}

Then :math:`\widehat{\mat{A}}` is solution of the linear system:

.. math::
:label: pbLeastSquares
    \Tr{\mat{Psi}}\mat{Psi}\widehat{\mat{A}} = \Tr{\mat{Psi}}\mat{Y}

The library relies on the method *dgelsy* of LAPACK: refer to its documentation to get
information on the resolution of :eq:`pbLeastSquares`, in particular when the problem is
underdetermined (which means when :math:`\sampleSize < N`) or overdetermined
(which means when :math:`\sampleSize > N`).

The Gram matrix :math:` \Tr{\mat{Psi}}\mat{Psi}` can be
ill-conditionned. Hence, the best method is not necessarily to invert the Gram matrix,
because the solution may be particularly sensitive to rounding errors.
The least-squares problem is rather solved using more robust numerical methods
such as the *singular value decomposition* (SVD) or the *QR-decomposition*.

Particular case 1: The functional basis is composed of linear functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In this particular case, the functional basis is defined by :math:`N = 1+\inputDim` linear functions:

  .. math::
    \psi_0 & : \vect{x} \rightarrow 1\\
    \psi_j & : \vect{x} \rightarrow x_j - b_j, \quad 1 \leq j \leq \inputDim

with :math:`\vect{b} \in \Rset^\inputDim` is the empirical mean vector of the experimental design, defined
by:

  .. math::
  :label defCenter
     \vect{b} = \dfrac{1}{\sampleSize} \sum_{i=1}^\sampleSize \vect{x}^{(i)}

The resulting meta model can be written as:

  .. math::
    :label: LinearLeastSquaresMMOpenTURNS
     \metaModel(\vect{x})  =  \vect{c} + \Tr{\mat{L}}(\vect{x} - \vect{b}}

where the matrix :math:`\mat{L} \in \cM_{\inputDim, \outputDim}` is
equal to the matrix :math:`\mat{A}` defined in :eq:`defAPsi` except its first line.
More precisely, we have:

  .. math::
    :label: LinearMatrixOpenTURNS
     \mat{L} =  \Tr{(\vect{a}_1 | \dots | \vect{a}_\inputDim)}

The vector :math:`\vect{c} = \vect{a}_0 \in \Rset^\outputDim` is the first line of  :math:`\mat{A}` defined in
:eq:`defAPsi`.

Particular case 2: The functional basis is composed of quadratic functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In this particular case, the functional basis is defined by :math:`N = 1+2\inputDim +\dfrac{d(d-1)}{2}` polynomials functions of maximal degree 2:

  .. math::
    \psi_0 & : \vect{x} \rightarrow 1\\
    \psi_k & : \vect{x} \rightarrow (x_k - b_k)\quad for 1 \leq k \leq \inputDim\\
    \psi_{ij} & : \vect{x} \rightarrow (x_i - b_i)(x_j - b_j), \quad 1 \leq i < j \leq \inputDim

where :math:`\vect{b}` is still defined by :eq:`defCenter`. The coefficients are denoted as follows:

  .. math::
      :label: QuadLSMetaModel
      \metaModel(\vect{x})  = \sum_{j=0}^\inputDim \vect{a}_j \Psi_j(\vect{x}) + \sum_{1 \leq i < j \leq \inputDim
      }^\inputDim \vect{a}_{ij}\Psi_{ij}(\vect{x})

The resulting meta model can be written as:

  .. math::
    :label: QuadraticLeastSquaresMMOpenTURNS
     \metaModel(\vect{x})  = \vect{c} + \vect{X} \mapsto \Tr{\mat{L}} ( \vect{X} - \vect{b} )
     + \frac{1}{2} \Tr{( \vect{X} - \vect{b} )}.\underline{\underline{\underline{M}}}. ( \vect{X} - \vect{b} )

where:

- :math:`\mat{L} \in \cM_{\inputDim, \outputDim}` is equal to the matrix :math:`\mat{A}` defined in :eq:`defAPsi`
  except its first line,
- :math:`\vect{c}= \vect{a}_0 \in \Rset^\outputDim`,
- :math:`\underline{\underline{\underline{M}}}` is a
  :math:`\Rset^\outputDim \times \Rset^\inputDim \times \Rset^\inputDim` symmetric tensor: the shhet :math:`k` is
  the matrix :math:`(2a_{ij}^k)_{1, i,j,\inputDim}`, for :math:`1 \leq k \leq \outputDim` and :math:`a_{ij}^k`
  is the component :math:`k` of the vector :math:`\vect{a}_{ij}`.



.. topic:: API:

    - See :class:`~openturns.LinearLeastSquares`
    - See :class:`~openturns.QuadraticLeastSquares`

.. topic:: Examples:

    - See :doc:`/auto_surrogate_modeling/linear_regression/plot_create_linear_least_squares_model`


.. topic:: References:

    - [bjork1996]_

