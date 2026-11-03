%define OT_CovarianceModel_doc
R"RAW(Covariance model.

Base class for covariance models.

Notes
-----
A covariance model is the covariance function
:math:`\mat{C}: \cD \times \cD \rightarrow \cM_{\processDim \times \processDim}(\Rset)`
of a second-order stochastic process
:math:`X: \Omega \times \cD \rightarrow \Rset^{\processDim}`,
where :math:`\Omega` is the set of events and
:math:`\cD \subset \Rset^{\domainDim}` is the index domain.
For all :math:`(\vect{s}, \vect{t}) \in \cD \times \cD`, the covariance
function is defined by

.. math::

    \mat{C}(\vect{s}, \vect{t})
    = \Cov{X_{\vect{s}}, X_{\vect{t}}}
    = \Expect{\left(X_{\vect{s}} - m(\vect{s})\right)
      \Tr{\left(X_{\vect{t}} - m(\vect{t})\right)}},

where :math:`m: \cD \rightarrow \Rset^{\processDim}` is the mean function
and :math:`X_{\vect{t}}` is the random variable at index :math:`\vect{t}`.
In the library, the covariance model is written as

.. math::

    \mat{C}(\vect{s}, \vect{t})
    = \rho\left(\frac{\vect{s}}{\vect{\theta}},
      \frac{\vect{t}}{\vect{\theta}}\right)
      \diag(\vect{\sigma}) \mat{R} \diag(\vect{\sigma}),

where :math:`\rho` is a scalar correlation function,
:math:`\vect{\theta} \in \Rset^{\domainDim}` is the scale parameter,
:math:`\vect{\sigma} \in \Rset^{\processDim}` is the amplitude parameter
and :math:`\mat{R} \in \cS_{\processDim}^+(\Rset)` is the spatial
correlation matrix.

The following :class:`~openturns.ResourceMap` keys are used:

- ``CovarianceModel-DefaultNuggetFactor`` (``Scalar``, default: ``1e-12``): default value of the nugget factor :math:`\varepsilon_{nugget}`.
- ``CovarianceModel-DefaultTMin`` (``Scalar``, default: ``-5.0``): default lower bound :math:`t_{\min}` used by :meth:`draw`.
- ``CovarianceModel-DefaultTMax`` (``Scalar``, default: ``5.0``): default upper bound :math:`t_{\max}` used by :meth:`draw`.
- ``CovarianceModel-DefaultPointNumber`` (``UnsignedInteger``, default: ``129``): default number of points used by :meth:`draw`.

See :any:`Covariance models <covariance_model>` for details.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation
OT_CovarianceModel_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_computeAsScalar_doc
R"RAW(Compute the covariance function for a scalar model.

Available usages:
    computeAsScalar(s, t)

    computeAsScalar(tau)

Parameters
----------
s, t : float or sequence of float
    Multivariate index :math:`(\vect{s}, \vect{t}) \in \cD \times \cD`.
    Scalars are accepted only if :math:`\domainDim = 1`.
tau : float or sequence of float
    Multivariate index :math:`\vect{\tau} \in \cD`.
    A scalar is accepted only if :math:`\domainDim = 1`.

Returns
-------
covariance : float
    Covariance value.

Notes
-----
The method makes sense only if the dimension of the process is
:math:`\processDim = 1`, in which case the covariance matrix
:math:`\mat{C}(\vect{s}, \vect{t})` reduces to the scalar covariance
:math:`C(\vect{s}, \vect{t}) = \mat{C}_{11}(\vect{s}, \vect{t})`.
In the second usage, the covariance model must be stationary. We then note
:math:`C^{stat}(\vect{\tau})` for
:math:`C(\vect{s}, \vect{s} + \vect{\tau})`, as this quantity does not
depend on :math:`\vect{s}`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::computeAsScalar
OT_CovarianceModel_computeAsScalar_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_computeCrossCovariance_doc
R"RAW(Compute the covariance function on two given meshes.

Parameters
----------
firstVertices : :class:`~openturns.Sample` or :class:`~openturns.Point`
    Container of the first discretization vertices.
secondVertices : :class:`~openturns.Sample` or :class:`~openturns.Point`
    Container of the second discretization vertices.

Returns
-------
crossCovariance : :class:`~openturns.Matrix`
    Cross-covariance matrix.

Notes
-----
The cross-covariance matrix is the evaluation of the covariance model on
both *firstVertices* and *secondVertices*.

Let :math:`N_1` and :math:`N_2` be the numbers of vertices in
*firstVertices* and *secondVertices*. The result is a
:math:`N_1\processDim \times N_2\processDim` matrix, i.e. a block matrix
whose :math:`(i, j)` block is
:math:`\mat{C}(\vect{x}_i, \vect{y}_j)`, where
:math:`\vect{x}_0, \dots, \vect{x}_{N_1-1}` are the points of
*firstVertices* and
:math:`\vect{y}_0, \dots, \vect{y}_{N_2-1}` are the points of
*secondVertices*. When :math:`\processDim = 1`, this is the
:math:`N_1 \times N_2` matrix with entries
:math:`\mat{C}(\vect{x}_i, \vect{y}_j)`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::computeCrossCovariance
OT_CovarianceModel_computeCrossCovariance_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_discretize_doc
R"RAW(Discretize the covariance function on a given mesh.

Parameters
----------
where : :class:`~openturns.Mesh` or :class:`~openturns.RegularGrid` or :class:`~openturns.Sample`
    Container of the discretization vertices.

Returns
-------
covarianceMatrix : :class:`~openturns.CovarianceMatrix`
    Covariance matrix
    :math:`\mat{C}_{1, \dots, N} \in \cS_{N\processDim}^+(\Rset)`,
    where :math:`N` is the number of vertices.

Notes
-----
Let :math:`\vect{t}_1, \dots, \vect{t}_N` be the vertices of the given
mesh. The method returns the block covariance matrix

.. math::

    \mat{C}_{1, \dots, N} =
    \begin{pmatrix}
    \mat{C}(\vect{t}_1, \vect{t}_1) & \dots &
    \mat{C}(\vect{t}_1, \vect{t}_N) \\
    \vdots & \ddots & \vdots \\
    \mat{C}(\vect{t}_N, \vect{t}_1) & \dots &
    \mat{C}(\vect{t}_N, \vect{t}_N)
    \end{pmatrix}.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::discretize
OT_CovarianceModel_discretize_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_discretizeAndFactorize_doc
R"RAW(Discretize and factorize the covariance function on a given mesh.

Parameters
----------
where : :class:`~openturns.Mesh` or :class:`~openturns.RegularGrid` or :class:`~openturns.Sample`
    Container of the discretization vertices.

Returns
-------
choleskyFactor : :class:`~openturns.TriangularMatrix`
    Lower triangular Cholesky factor :math:`\mat{L}` of the covariance
    matrix, such that
    :math:`\mat{C}_{1, \dots, N} = \mat{L}\Tr{\mat{L}}`.

Notes
-----
This method discretizes the model on the given
:class:`~openturns.Mesh`, :class:`~openturns.RegularGrid` or
:class:`~openturns.Sample` with the :meth:`discretize` method and returns
its Cholesky factor.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::discretizeAndFactorize
OT_CovarianceModel_discretizeAndFactorize_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_discretizeHMatrix_doc
R"RAW(Discretize the covariance function on a given mesh using HMatrix.

Parameters
----------
where : :class:`~openturns.Mesh` or :class:`~openturns.RegularGrid` or :class:`~openturns.Sample`
    Container of the discretization vertices.
hmatParam : :class:`~openturns.HMatrixParameters`
    Parameter values for the HMatrix.

Returns
-------
covarianceMatrix : :class:`~openturns.HMatrix`
    Covariance matrix
    :math:`\mat{C}_{1, \dots, N} \in \cS_{N\processDim}^+(\Rset)`,
    stored in hierarchical format (H-Matrix), where :math:`N` is the
    number of vertices.

Notes
-----
This method is similar to the :meth:`discretize` method.
It requires that the library has been compiled with HMAT support.
It is helpful for very large discretization sizes because it compresses
the data.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::discretizeHMatrix
OT_CovarianceModel_discretizeHMatrix_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_discretizeAndFactorizeHMatrix_doc
R"RAW(Discretize and factorize the covariance function on a given mesh using HMatrix.

Parameters
----------
where : :class:`~openturns.Mesh` or :class:`~openturns.RegularGrid` or :class:`~openturns.Sample`
    Container of the discretization vertices.
hmatParam : :class:`~openturns.HMatrixParameters`
    Parameter values for the HMatrix.

Returns
-------
choleskyFactor : :class:`~openturns.HMatrix`
    Lower triangular Cholesky factor :math:`\mat{L}` of the covariance
    matrix, such that
    :math:`\mat{C}_{1, \dots, N} = \mat{L}\Tr{\mat{L}}`, stored in
    hierarchical format (H-Matrix).

Notes
-----
This method is similar to the :meth:`discretizeAndFactorize` method.
It requires that the library has been compiled with HMAT support.
It is helpful for very large discretization sizes because it compresses
the data.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::discretizeAndFactorizeHMatrix
OT_CovarianceModel_discretizeAndFactorizeHMatrix_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_discretizeHODLRMatrix_doc
R"RAW(Discretize the covariance function on a given mesh using HODLRMatrix.

Parameters
----------
where : :class:`~openturns.Mesh` or :class:`~openturns.RegularGrid` or :class:`~openturns.Sample`
    Container of the discretization vertices.
hodlrParam : :class:`~openturns.experimental.HODLRMatrixParameters`
    Parameter values for the HODLR matrix.

Returns
-------
covarianceMatrix : :class:`~openturns.experimental.HODLRMatrix`
    Covariance matrix
    :math:`\mat{C}_{1, \dots, N} \in \cS_{N\processDim}^+(\Rset)`,
    stored in hierarchical format (HODLR), where :math:`N` is the number
    of vertices.

Notes
-----
This method is similar to the :meth:`discretize` method, but stores the
result in a :class:`~openturns.experimental.HODLRMatrix` instead of a
dense :class:`~openturns.CovarianceMatrix`.
It is helpful for very large discretization sizes because it compresses
the data. It does not require any external library.
The result must be factorized with the
:py:meth:`~openturns.experimental.HODLRMatrix.factorize` method before
being used by :py:meth:`~openturns.experimental.HODLRMatrix.solve` or
:py:meth:`~openturns.experimental.HODLRMatrix.logDeterminant`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::discretizeHODLRMatrix
OT_CovarianceModel_discretizeHODLRMatrix_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_discretizeAndFactorizeHODLRMatrix_doc
R"RAW(Discretize and factorize the covariance function on a given mesh using HODLRMatrix.

Parameters
----------
where : :class:`~openturns.Mesh` or :class:`~openturns.RegularGrid` or :class:`~openturns.Sample`
    Container of the discretization vertices.
hodlrParam : :class:`~openturns.experimental.HODLRMatrixParameters`
    Parameter values for the HODLR matrix.

Returns
-------
choleskyFactor : :class:`~openturns.experimental.HODLRMatrix`
    Lower triangular Cholesky factor :math:`\mat{L}` of the covariance
    matrix, such that
    :math:`\mat{C}_{1, \dots, N} = \mat{L}\Tr{\mat{L}}`, stored in
    hierarchical format (HODLR).

Notes
-----
This method is similar to the :py:meth:`discretizeAndFactorize` method, but
stores the result in a :class:`~openturns.experimental.HODLRMatrix`
instead of a dense :class:`~openturns.TriangularMatrix`.
It does not require any external library.
It is helpful for very large discretization sizes because it compresses
the data.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::discretizeAndFactorizeHODLRMatrix
OT_CovarianceModel_discretizeAndFactorizeHODLRMatrix_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_discretizeRow_doc
R"RAW(Discretize the covariance function on a given mesh for a specific row.

Parameters
----------
vertices : :class:`~openturns.Sample`
    Container of the discretization vertices
    :math:`(\vect{t}_1, \dots, \vect{t}_N)`.
p : int, :math:`0 \leq p \leq N - 1`
    Row index of the covariance matrix to discretize.

Returns
-------
result : :class:`~openturns.Sample`
    Sample of size :math:`N\processDim` and dimension
    :math:`\processDim`, whose :math:`i`-th block of
    :math:`\processDim` rows contains the covariance matrix
    :math:`\mat{C}(\vect{t}_i, \vect{t}_p)`, for
    :math:`i = 1, \dots, N`.

Notes
-----
When :math:`\processDim = 1`, the result is a sample of size
:math:`N` and dimension 1 with entries
:math:`C(\vect{t}_i, \vect{t}_p) = \mat{C}_{11}(\vect{t}_i, \vect{t}_p)`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::discretizeRow
OT_CovarianceModel_discretizeRow_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getAmplitude_doc
R"RAW(Get the amplitude parameter :math:`\vect{\sigma}` of the covariance function.

Returns
-------
amplitude : :class:`~openturns.Point`
    Amplitude parameter :math:`\vect{\sigma} \in
    \Rset^{\processDim}`. Its components are positive.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getAmplitude
OT_CovarianceModel_getAmplitude_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getOutputDimension_doc
R"RAW(Get the output dimension of the covariance function.

Returns
-------
outputDimension : int
    Dimension :math:`\processDim` of the process :math:`X`.
    It is such that
    :math:`\mat{C}: \cD \times \cD \rightarrow
    \cM_{\processDim \times \processDim}(\Rset)`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getOutputDimension
OT_CovarianceModel_getOutputDimension_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_get_marginal
"Get the marginal covariance model.

Parameters
----------
index : int or sequence of int
    Index or indices of the components of the process to keep.

Returns
-------
marginal : :class:`~openturns.CovarianceModel`
    Covariance model restricted to the selected components."
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getMarginal
OT_CovarianceModel_get_marginal

// ---------------------------------------------------------------------

%define OT_CovarianceModel_get_nugget_factor_doc
R"RAW(Accessor to the nugget factor.

The nugget factor :math:`\varepsilon_{nugget}` is a dimensionless
nonnegative parameter which models a noise on the observed values of the
process. At :math:`\vect{s} = \vect{t}`, it replaces the covariance
matrix :math:`\mat{C}(\vect{s}, \vect{s})` by
:math:`(1 + \varepsilon_{nugget}) \mat{C}(\vect{s}, \vect{s})`.

Returns
-------
nuggetFactor : float
    Nugget factor :math:`\varepsilon_{nugget} \geq 0`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getNuggetFactor
OT_CovarianceModel_get_nugget_factor_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getParameter_doc
R"RAW(Get the values of the active parameters of the covariance function.

Returns
-------
parameter : :class:`~openturns.Point`
    Values of the :math:`n_p` active parameters, in the order given by
    :meth:`getParameterDescription`.

Notes
-----
The active parameters are selected by :meth:`setActiveParameter` among the
full parameters returned by :meth:`getFullParameter`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getParameter
OT_CovarianceModel_getParameter_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getParameterDescription_doc
"Get the description of the active parameters of the covariance function.

Returns
-------
descriptionParam : :class:`~openturns.Description`
    Description of the active parameters, i.e. the components of
    :meth:`getFullParameterDescription` selected by
    :meth:`getActiveParameter`."
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getParameterDescription
OT_CovarianceModel_getParameterDescription_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getScale_doc
R"RAW(Get the scale parameter :math:`\vect{\theta}` of the covariance function.

Returns
-------
scale : :class:`~openturns.Point`
    Scale parameter :math:`\vect{\theta} \in
    \Rset^{\domainDim}` used in the covariance function.
    Its components are positive.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getScale
OT_CovarianceModel_getScale_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getOutputCorrelation_doc
R"RAW(Get the spatial correlation matrix :math:`\mat{R}` of the covariance function.

Returns
-------
spatialCorrelation : :class:`~openturns.CorrelationMatrix`
    Spatial correlation matrix
    :math:`\mat{R} \in \cS_{\processDim}^+(\Rset)`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getOutputCorrelation
OT_CovarianceModel_getOutputCorrelation_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getInputDimension_doc
R"RAW(Get the input dimension of the covariance function.

Returns
-------
inputDimension : int
    Dimension :math:`\domainDim` of the index domain :math:`\cD`.
    It is such that :math:`\cD \subset \Rset^{\domainDim}`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getInputDimension
OT_CovarianceModel_getInputDimension_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_isDiagonal_doc
R"RAW(Test whether the model is diagonal.

Returns
-------
isDiagonal : bool
    ``True`` if the covariance matrix is diagonal, i.e.

    .. math::

        \mat{C}(\vect{s}, \vect{t}) =
        \diag\left(\mat{C}_{11}(\vect{s}, \vect{t}), \dots,
        \mat{C}_{\processDim\processDim}(\vect{s}, \vect{t})\right).
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::isDiagonal
OT_CovarianceModel_isDiagonal_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_isStationary_doc
R"RAW(Test whether the model is stationary.

Returns
-------
isStationary : bool
    ``True`` if the covariance function is invariant by translation.

Notes
-----
The covariance function is stationary when it depends only on the lag
:math:`\vect{\tau} = \vect{t} - \vect{s}`:

.. math::

    \mat{C}(\vect{s}, \vect{t})
    = \mat{C}^{stat}(\vect{t} - \vect{s}).

In that case, we note :math:`\mat{C}^{stat}(\vect{\tau})` for
:math:`\mat{C}(\vect{s}, \vect{s} + \vect{\tau})`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::isStationary
OT_CovarianceModel_isStationary_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_partialGradient_doc
R"RAW(Compute the gradient of the covariance function with respect to the first index :math:`\vect{s}`.

Parameters
----------
s, t : float or sequence of float
    Multivariate index :math:`(\vect{s}, \vect{t}) \in \cD \times \cD`.
    Scalars are accepted only if :math:`\domainDim = 1`.

Returns
-------
gradient : :class:`~openturns.Matrix`
    Gradient of the covariance function with respect to :math:`\vect{s}`.
    Its size is :math:`\domainDim \times \processDim^2`.
    For :math:`0 \leq a < \domainDim` and
    :math:`0 \leq i, j < \processDim`, row :math:`a` and column
    :math:`i + \processDim j` hold
    :math:`\partial \mat{C}_{ij} / \partial s_a`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::partialGradient
OT_CovarianceModel_partialGradient_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_partialHessian_doc
R"RAW(Compute the Hessian of the covariance function with respect to the first index :math:`\vect{s}`.

Parameters
----------
s, t : float or sequence of float
    Multivariate index :math:`(\vect{s}, \vect{t}) \in \cD \times \cD`.
    Scalars are accepted only if :math:`\domainDim = 1`.

Returns
-------
hessian : :class:`~openturns.SymmetricMatrix`
    Hessian of the :math:`(0, 0)` output component of the covariance
    function with respect to :math:`\vect{s}`. Its size is
    :math:`\domainDim \times \domainDim`; entry :math:`(a, b)` holds
    :math:`\partial^2 \mat{C}_{00} / \partial s_a \partial s_b`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::partialHessian
OT_CovarianceModel_partialHessian_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_parameterGradient_doc
R"RAW(Compute the gradient of the covariance function with respect to the active parameters.

Parameters
----------
s, t : float or sequence of float
    Multivariate index :math:`(\vect{s}, \vect{t}) \in \cD \times \cD`.
    Scalars are accepted only if :math:`\domainDim = 1`.

Returns
-------
gradient : :class:`~openturns.Matrix`
    Gradient of the covariance function with respect to the
    :math:`n_p` active parameters
    :math:`\pi_1, \dots, \pi_{n_p}`.
    Its size is :math:`n_p \times \processDim^2`.
    For :math:`0 \leq k < n_p` and
    :math:`0 \leq i, j < \processDim`, row :math:`k` and column
    :math:`i + \processDim j` hold
    :math:`\partial \mat{C}_{ij} / \partial \pi_k`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::parameterGradient
OT_CovarianceModel_parameterGradient_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_thirdOrderPartial_doc
R"RAW(Compute the third-order spatial derivatives of the covariance function.

Parameters
----------
s, t : float or sequence of float
    Multivariate index :math:`(\vect{s}, \vect{t}) \in \cD \times \cD`.
    Scalars are accepted only if :math:`\domainDim = 1`.

Returns
-------
third : :class:`~openturns.Point`
    Flat third-order derivatives of the :math:`(0, 0)` output component.
    Its size is :math:`\domainDim^3`; for
    :math:`0 \leq a, b, c < \domainDim`, index
    :math:`a + \domainDim (b + \domainDim c)` holds
    :math:`\partial^3 \mat{C}_{00} /
    \partial s_a \partial s_b \partial s_c`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::thirdOrderPartial
OT_CovarianceModel_thirdOrderPartial_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_fourthOrderPartial_doc
R"RAW(Compute the fourth-order spatial derivatives of the covariance function.

Parameters
----------
s, t : float or sequence of float
    Multivariate index :math:`(\vect{s}, \vect{t}) \in \cD \times \cD`.
    Scalars are accepted only if :math:`\domainDim = 1`.

Returns
-------
fourth : :class:`~openturns.Point`
    Flat fourth-order derivatives of the :math:`(0, 0)` output component.
    Its size is :math:`\domainDim^4`; for
    :math:`0 \leq a, b, c, e < \domainDim`, index
    :math:`a + \domainDim (b + \domainDim (c + \domainDim e))` holds
    :math:`\partial^4 \mat{C}_{00} /
    \partial s_a \partial s_b \partial s_c \partial s_e`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::fourthOrderPartial
OT_CovarianceModel_fourthOrderPartial_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_parameterHessian_doc
R"RAW(Compute the mixed parameter-spatial Hessian of the covariance function.

Parameters
----------
s, t : float or sequence of float
    Multivariate index :math:`(\vect{s}, \vect{t}) \in \cD \times \cD`.
    Scalars are accepted only if :math:`\domainDim = 1`.

Returns
-------
hessian : :class:`~openturns.Matrix`
    Derivatives of the partial Hessian with respect to the
    :math:`n_p` active parameters
    :math:`\pi_1, \dots, \pi_{n_p}`.
    Its size is :math:`n_p \times \domainDim^2`.
    For :math:`0 \leq k < n_p` and
    :math:`0 \leq a, b < \domainDim`, row :math:`k` and column
    :math:`a + \domainDim b` hold
    :math:`\partial^3 \mat{C}_{00} /
    \partial \pi_k \partial s_a \partial s_b`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::parameterHessian
OT_CovarianceModel_parameterHessian_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_setAmplitude_doc
R"RAW(Set the amplitude parameter :math:`\vect{\sigma}` of the covariance function.

Parameters
----------
amplitude : :class:`~openturns.Point`
    Amplitude parameter :math:`\vect{\sigma} \in
    \Rset^{\processDim}` to be used in the covariance function.
    Its size must be equal to :math:`\processDim` and its components must
    be positive.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::setAmplitude
OT_CovarianceModel_setAmplitude_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_setScale_doc
R"RAW(Set the scale parameter :math:`\vect{\theta}` of the covariance function.

Parameters
----------
scale : :class:`~openturns.Point`
    Scale parameter :math:`\vect{\theta} \in
    \Rset^{\domainDim}` to be used in the covariance function.
    Its size must be equal to :math:`\domainDim` and its components must
    be positive.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::setScale
OT_CovarianceModel_setScale_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_set_nugget_factor_doc
R"RAW(Set the nugget factor for the variance of the observation error.

Parameters
----------
nuggetFactor : float, :math:`\varepsilon_{nugget} \geq 0`
    Nugget factor.

Notes
-----
The nugget factor represents a noise observed in the output values of the
process. At :math:`\vect{s} = \vect{t}`, it replaces the covariance
matrix :math:`\mat{C}(\vect{s}, \vect{s})` by
:math:`(1 + \varepsilon_{nugget}) \mat{C}(\vect{s}, \vect{s})`.

Refer to :any:`Covariance models <covariance_model>` to get more details
on covariance models and the introduction of a nugget factor, and in
particular see equation :eq:`Css`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::setNuggetFactor
OT_CovarianceModel_set_nugget_factor_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_setParameter_doc
R"RAW(Set the values of the active parameters of the covariance function.

Parameters
----------
parameter : :class:`~openturns.Point`
    Values of the :math:`n_p` active parameters, in the order given by
    :meth:`getParameterDescription`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::setParameter
OT_CovarianceModel_setParameter_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_setOutputCorrelation_doc
R"RAW(Set the spatial correlation matrix :math:`\mat{R}` of the covariance function.

Parameters
----------
spatialCorrelation : :class:`~openturns.CorrelationMatrix`
    Spatial correlation matrix
    :math:`\mat{R} \in \cS_{\processDim}^+([-1, 1])`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::setOutputCorrelation
OT_CovarianceModel_setOutputCorrelation_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_setActiveParameter_doc
R"RAW(Accessor to the active parameter set.

This method defines which hyperparameters are tuned.

Parameters
----------
active : sequence of int
    Indices of the active parameters among the full parameter vector.

Notes
-----
The indices refer to the full parameter vector returned by
:meth:`getFullParameter`. In order to get the position of a parameter known
by its name, first call the method :meth:`getFullParameterDescription` and
then use the *index* method of the returned
:class:`~openturns.Description`, as shown in the following example.

Examples
--------
For example, we create a :class:`~openturns.MaternModel` covariance model.
By default, only the scale and the amplitude parameters are active and
will be estimated. We want to add the estimation of the :math:`\nu`
parameter.

>>> import openturns as ot
>>> covModel = ot.MaternModel()

We get the default active parameters description: we can see that the
:math:`\nu` parameter is not included in the list.

>>> covModel.getParameterDescription()
[scale_0,amplitude_0]

We now get the active parameters. The parameter :math:`\nu` is not in the
list.

>>> active = covModel.getActiveParameter()
>>> print(active)
[0,2]

We want to search the index associated to the parameter :math:`\nu` in the
full list of the parameters indices.

>>> activeDesc = covModel.getFullParameterDescription()
>>> print(activeDesc)
[scale_0,nuggetFactor,amplitude_0,nu]
>>> index = activeDesc.index('nu')
>>> print(index)
3

Now, we add the index of :math:`\nu` in the initial list of active
parameters. We sort the active indices by increasing order. Then we update
the list of the active parameters.

>>> active = covModel.getActiveParameter()
>>> active.add(index)
>>> covModel.setActiveParameter(sorted(active))

Check that the parameter :math:`\nu` is in the active parameters list!

>>> covModel.getActiveParameter()
[0,2,3]

>>> covModel.getFullParameterDescription()
[scale_0,nuggetFactor,amplitude_0,nu]
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::setActiveParameter
OT_CovarianceModel_setActiveParameter_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getActiveParameter_doc
"Accessor to the active parameter set.

In the context of kriging, it allows one to choose which hyperparameters
are tuned.

Returns
-------
active : :class:`~openturns.Indices`
    Indices of the active parameters among the full parameter vector."
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getActiveParameter
OT_CovarianceModel_getActiveParameter_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_activateScale_doc
"Activate/deactivate the scale parameters.

In the context of kriging, defines whether the scale parameters should be
tuned.

Parameters
----------
isScaleActive : bool
    If ``True``, all the scale parameters are tuned.
    If ``False``, none of them is tuned."
%enddef
%feature("docstring") OT::CovarianceModelImplementation::activateScale
OT_CovarianceModel_activateScale_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_activateNuggetFactor_doc
"Activate/deactivate the nugget factor.

In the context of kriging, defines whether the nugget factor should be
tuned.

Parameters
----------
isNuggetFactorActive : bool
    If ``True`` (respectively ``False``), the nugget factor is
    (respectively is not) tuned."
%enddef
%feature("docstring") OT::CovarianceModelImplementation::activateNuggetFactor
OT_CovarianceModel_activateNuggetFactor_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_activateAmplitude_doc
"Activate/deactivate the amplitude parameters.

In the context of kriging, defines whether the amplitude parameters should
be tuned.

Parameters
----------
isAmplitudeActive : bool
    If ``True``, all the amplitude parameters are tuned.
    If ``False``, none of them is tuned."
%enddef
%feature("docstring") OT::CovarianceModelImplementation::activateAmplitude
OT_CovarianceModel_activateAmplitude_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_draw_doc
R"RAW(Draw a specific component of the covariance model with input dimension 1.

Parameters
----------
rowIndex : int, :math:`0 \leq rowIndex < \processDim`
    Row index of the component to draw. Default value is 0.
columnIndex : int, :math:`0 \leq columnIndex < \processDim`
    Column index of the component to draw. Default value is 0.
tMin : float
    Lower bound :math:`t_{\min}` of the range over which the model is
    plotted. Default value is ``CovarianceModel-DefaultTMin`` in
    :class:`~openturns.ResourceMap`.
tMax : float
    Upper bound :math:`t_{\max}` of the range over which the model is
    plotted. Default value is ``CovarianceModel-DefaultTMax`` in
    :class:`~openturns.ResourceMap`.
pointNumber : int, :math:`pointNumber \geq 2`
    Number of points used to discretize the range
    :math:`[t_{\min}, t_{\max}]`. Default value is
    ``CovarianceModel-DefaultPointNumber`` in
    :class:`~openturns.ResourceMap`.
asStationary : bool
    If ``True``, the model is plotted as a function of the lag
    :math:`\tau = t - s` when the model is stationary. If ``False``, it is
    plotted as a function of :math:`(s, t)`. Default value is ``True``.
correlationFlag : bool
    If ``True``, the model is plotted as a correlation function. If
    ``False``, it is plotted as a covariance function. Default value is
    ``False``.

Returns
-------
graph : :class:`~openturns.Graph`
    A graph containing a unique curve if *asStationary* is ``True`` and if
    the model is actually stationary, or the iso-values of the model
    otherwise.

Notes
-----
This method requires :math:`\domainDim = 1`.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::draw
OT_CovarianceModel_draw_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_setFullParameter_doc
R"RAW(Set the full parameters of the covariance function.

Parameters
----------
parameter : :class:`~openturns.Point`
    Full parameter vector. In the base class, its components are, in
    order:

    - the scale vector :math:`\vect{\theta} \in
      \Rset^{\domainDim}`;
    - the nugget factor :math:`\varepsilon_{nugget} \geq 0`;
    - the amplitude vector :math:`\vect{\sigma} \in
      \Rset^{\processDim}`;
    - the lower triangle of the spatial correlation matrix
      :math:`\mat{R} \in \cS_{\processDim}^+([-1, 1])`, i.e.
      :math:`\processDim(\processDim - 1) / 2` values.

    Its size is therefore at least
    :math:`\domainDim + 1 + \processDim(\processDim + 1) / 2`.
    Models may append model-specific parameters.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::setFullParameter
OT_CovarianceModel_setFullParameter_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getFullParameter_doc
R"RAW(Get the full parameters of the covariance function.

Returns
-------
parameter : :class:`~openturns.Point`
    Full parameter vector, of size at least
    :math:`\domainDim + 1 + \processDim(\processDim + 1) / 2`.
    In the base class, its components are, in order, the scale vector
    :math:`\vect{\theta}`, the nugget factor
    :math:`\varepsilon_{nugget}`, the amplitude vector
    :math:`\vect{\sigma}` and the lower triangle of the spatial
    correlation matrix :math:`\mat{R}`. Models may append model-specific
    parameters.
)RAW"
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getFullParameter
OT_CovarianceModel_getFullParameter_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_getFullParameterDescription_doc
"Get the description of the full parameters of the covariance function.

Returns
-------
description : :class:`~openturns.Description`
    Description of the full parameter vector, in the order returned by
    :meth:`getFullParameter`."
%enddef
%feature("docstring") OT::CovarianceModelImplementation::getFullParameterDescription
OT_CovarianceModel_getFullParameterDescription_doc

// ---------------------------------------------------------------------

%define OT_CovarianceModel_isParallel_doc
"Accessor to the parallel flag.

Returns
-------
isParallel : bool
    Whether the object is considered thread-safe."
%enddef
%feature("docstring") OT::CovarianceModelImplementation::isParallel
OT_CovarianceModel_isParallel_doc
