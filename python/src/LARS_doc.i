%feature("docstring") OT::LARS
R"RAW(Least Angle Regression.

Refer to :any:`sparse_least_squares`.

See also
--------
BasisSequenceFactory

Notes
-----
LARS inherits from :class:`~openturns.BasisSequenceFactory`.

If the size :math:`P` of the PC basis is of similar size to :math:`N`, or even
possibly significantly larger than :math:`N` , then the following ordinary least
squares problem is ill-posed:

.. math::

    \vect{a} = \argmin_{\vect{b} \in \Rset^P} E_{\mu} \left[ \left( g \circ T^{-1}
            (\vect{U}) - \vect{b}^{\intercal} \vect{\Psi}(\vect{U}) \right)^2 \right]

The sparse least squares approaches may be employed instead. Eventually a sparse
PC representation is obtained, that is an approximation which only contains a
small number of active basis functions.

With a weighted least-squares method the selection uses the weighted
correlations :math:`\vect{c} = \mat{\Phi}^\intercal \mat{W} \vect{r}` and
the weighted Gram system :math:`\mat{G}_A = \mat{\Phi}_A^\intercal \mat{W} \mat{\Phi}_A`,
ie the discretization of the :math:`L^2` inner product with the quadrature
weights of the method.

Examples
--------
>>> import openturns as ot
>>> from openturns.usecases import ishigami_function
>>> im = ishigami_function.IshigamiModel()
>>> # Create the orthogonal basis
>>> polynomialCollection = [ot.LegendreFactory()] * im.dim
>>> enumerateFunction = ot.LinearEnumerateFunction(im.dim)
>>> productBasis = ot.OrthogonalProductPolynomialFactory(polynomialCollection, enumerateFunction)
>>> # experimental design
>>> samplingSize = 75
>>> experiment = ot.LowDiscrepancyExperiment(ot.SobolSequence(), im.distribution, samplingSize)
>>> # generate sample
>>> x = experiment.generate()
>>> y = im.model(x)
>>> # iso transfo
>>> xToU = ot.DistributionTransformation(im.distribution, productBasis.getMeasure())
>>> u = xToU(x)
>>> # build basis
>>> degree = 10
>>> basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)
>>> basis = [productBasis.build(i) for i in range(basisSize)]
>>> # run algorithm
>>> factory = ot.BasisSequenceFactory(ot.LARS())
>>> seq = factory.build(u, y, basis, list(range(basisSize)))
)RAW"
