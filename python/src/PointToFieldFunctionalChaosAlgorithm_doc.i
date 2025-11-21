%feature("docstring") OT::PointToFieldFunctionalChaosAlgorithm
R"RAW(Functional metamodel algorithm based on chaos decomposition.

The present algorithm makes it possible to build a response surface of the
function :math:`h` defined by the equation:

.. math::
    h: \left|
      \begin{array}{ccl}
         \Rset^p & \rightarrow & \cM_N \times (\Rset^d)^N \\
         \vect{X} & \mapsto & \mat{Y}
      \end{array}
    \right.

with :math:`\cM_N` a mesh of :math:`\cD \subset \Rset^n`.

The mapping :math:`h` is known from the :math:`N` vectors :math:`(\vect{X}_1, \dots, \vect{X}_N)`
of the associated input random vector :math:`\vect{X}` and :math:`N` fields :math:`(\mat{Y}_1,\dots,\mat{Y}_N)`
that fully characterize the output process :math:`\mat{Y}`.

The linear projection function :math:`\pi_{ \vect{\lambda}, \vect{\varphi}}` of
the Karhunen-Loeve decomposition by SVD is used to project the output fields
(see :class:`~openturns.KarhunenLoeveSVDAlgorithm` and :class:`~openturns.KarhunenLoeveAlgorithm` for the notations):

.. math::
    \pi_{\vect{\lambda}, \vect{\varphi}}: \left|
      \begin{array}{ccl}
        L^2(\cD, \Rset^d) & \rightarrow & \cS^{\Nset} \\
        f & \mapsto &\left(\dfrac{1}{\sqrt{\lambda_k}}\int_{\cD}f(\vect{t}) \vect{\varphi}_k(\vect{t})\, d\vect{t}\right)_{k \geq 1}
      \end{array}
    \right.

where :math:`\cS^{\Nset}  = \left \{ (\zeta_k)_{k \geq 1} \in  \Rset^{\Nset} \, | \, \sum_{k=1}^{\infty}\lambda_k \zeta_k^2 < +\infty \right \}`.

The Karhunen-Loeve algorithm allows one to replace this integral by a specific weighted and finite sum
and to write the projections of the j-th marginal of i-th output field :math:`\mat{Y}_i^j` by multiplication
with the projection matrix :math:`\mat{M}_j \in \Rset^{K_j} \times \Rset^{Nd}`:

.. math::

    \mat{M}_j \mat{Y}_i^j = \left( \begin{array}{l} \xi_1^j \\ \vdots \\ \xi_{K_j}^j \end{array} \right)
    \in \Rset^{K_j}, \quad \forall i \in [1, n], \forall j \in [1, d]

with :math:`K_j` the retained number of modes in the decomposition of the j-th output.
The projections of all the :math:`d` components of :math:`n` fields are assembled in the :math:`\mat{Q}` matrix:

.. math::

    \begin{align*}
        \mat{Q} = \mat{M} \mat{Y} =
        \left(
          \begin{array}{l}
            \mat{M}_1 \mat{Y}^1 \\
            \vdots \\
            \mat{M}_d \mat{Y}^d
          \end{array}
        \right) \in \Rset^{K_T} \times \Rset^n
     \end{align*}

with :math:`K_T = \sum_{j=1}^d{K_j}` the total number of modes across output components.

Then a functional chaos decomposition is built between the input samples :math:`\vect{X}`
and the projected modes sample :math:`\mat{Q}`.
The :class:`~openturns.ResourceMap` string entry `PointToFieldFunctionalChaosAlgorithm-Expansion`
allows one to switch between :class:`~openturns.LeastSquaresExpansion` and :class:`~openturns.FunctionalChaosAlgorithm`.

.. math::

    \tilde{g}(x) = \sum_{k=1}^{K_c} \beta_{\vect{\alpha}_k} \Psi_{\vect{\alpha}_k}(x)

with :math:`K_c` the number of terms in the chaos decomposition.

The final metamodel consists in the composition of the functional chaos metamodel
and the Karhunen-Loeve projections.

.. math::
    \tilde{h}: \left|
      \begin{array}{ccccl}
         \Rset^p & \rightarrow & \Rset^{K_T} & \rightarrow & \cM_N \times (\Rset^d)^N\\
         \vect{X} & \mapsto & \vect{Q} & \mapsto & \mat{Y}
      \end{array}
    \right.

Here the projected modes sample has a dimension :math:`K_T` but being on the output side
of the functional chaos expansion (FCE) this approach is not affected by the curse of dimensionality,
it will just require as much FCE marginals runs.

Parameters
----------
x : :class:`~openturns.Sample`
    Input sample.
y : :class:`~openturns.ProcessSample`
    Output process sample.
distribution : :class:`~openturns.Distribution`
    Input distribution

See Also
--------
FieldToPointFunctionalChaosAlgorithm
FieldFunctionalChaosSobolIndices
KarhunenLoeveAlgorithm
FunctionalChaosAlgorithm

Notes
-----
As the output process decomposition is done with the values decomposition approach,
it is possible to control the number of modes retained per output,
the idea being to avoid a large output dimension for the chaos decomposition step.
As done in :class:`~openturns.KarhunenLoeveSVDAlgorithm`, the :func:`setThreshold`
and :func:`setNbModes` methods allow one to control the spectrum ratio and maximum count.

In the case of homogenous data (if variables have the same unit or scale),
it is also possible to recompress the modes at the global level with :func:`setRecompress`.
When enabled, the eigenvalues are gathered and sorted so as to find a global
spectrum cut-off value by which the spectrum of each output is truncated.
The default value can be set through the :class:`~openturns.ResourceMap` key
*FieldFunctionalChaosAlgorithm-DefaultRecompress*.

For the chaos metamodel step, it is possible to specify the basis size with
the :class:`~openturns.ResourceMap` key *FunctionalChaosAlgorithm-BasisSize*.

It is possible to partition output variables into independent blocks with :func:`setBlockIndices`.
This way Karhunen-Loeve process decompositions are done on each block rather
than on all output variables at once (at the price of increased chaos decomposition output dimension)
which then makes sensitivity analysis possible for each output variable
or group of output variables.

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> mesh = ot.RegularGrid(0.0, 0.1, 20)
>>> X = ot.Normal(4)
>>> x = X.getSample(50)
>>> g = ot.SymbolicFunction(['t', 'x1', 'x2', 'x3', 'x4'], ['x1 + x2 * sin(t)', 'x2 + x3 * cos(t)', 'x4 * t'])
>>> f = ot.VertexValuePointToFieldFunction(g, mesh)
>>> y = f(x)
>>> algo = ot.PointToFieldFunctionalChaosAlgorithm(x, y, X)
>>> algo.setThreshold(4e-2)
>>> # Temporarily lower the basis size for the sake of this example.
>>> ot.ResourceMap.SetAsUnsignedInteger('FunctionalChaosAlgorithm-BasisSize', 100)
>>> algo.run()
>>> result = algo.getResult()
>>> metamodel = result.getPointToFieldMetaModel()
>>> y0hat = metamodel(x[0])
>>> ot.ResourceMap.Reset()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PointToFieldFunctionalChaosAlgorithm::getInputSample
"Accessor to the input sample.

Returns
-------
inputSample : :class:`~openturns.Sample`
    Input sample."

// ---------------------------------------------------------------------

%feature("docstring") OT::PointToFieldFunctionalChaosAlgorithm::getOutputProcessSample
"Accessor to the output sample.

Returns
-------
outputSample : :class:`~openturns.ProcessSample`
    Output sample."

// ---------------------------------------------------------------------

%feature("docstring") OT::PointToFieldFunctionalChaosAlgorithm::run
"Compute the response surfaces.

Notes
-----
It computes the response surfaces and creates a
:class:`~openturns.MetaModelResult` structure containing all the results."
