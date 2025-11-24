%feature("docstring") OT::ConditionedGaussianProcess
R"RAW(Conditioned Gaussian process.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
gprResult : :class:`~openturns.GaussianProcessRegressionResult`
    Structure that contains all the elements of Gaussian Process Regression computations.
mesh : :class:`~openturns.Mesh`
    Mesh :math:`\cM` over which the domain :math:`\cD` is discretized.

Notes
-----
This class helps to generate fields from the conditioned gaussian process resulting
from a Gaussian process regression algorithm.

Refer to the documentation of :class:`~openturns.GaussianProcessRegression` to get details on
the notations.

Examples
--------
We consider the model :math:`g: \Rset^2 \rightarrow \Rset` defined by :math:`g(x,y) = cos(x/2) + sin(y)`.

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> model = ot.SymbolicFunction(['x', 'y'], ['cos(0.5*x) + sin(y)'])

Then we define the train sample as a box with 8 levels on the x-axis and 5 levels on the y-axis.

>>> levels = [8.0, 5.0]
>>> box = ot.Box(levels)
>>> x_train = box.generate()
>>> x_train *= 10
>>> y_train = model(x_train)

We define the covariance model as a :class:`~openturns.SquaredExponential` model:

>>> dim = 2
>>> covarianceModel = ot.SquaredExponential([1.0, 1.0], [1.0])

We define the functions basis to estimate the trend: this basis only contains constant functions:

>>> basis = ot.ConstantBasisFactory(dim).build()

We estimate the Gaussian process regression, using first the class
:class:`~openturns.GaussianProcessFitter`, then
using the class :class:`~openturns.GaussianProcessRegression`:

>>> fitter_algo = ot.GaussianProcessFitter(x_train, y_train, covarianceModel, basis)
>>> fitter_algo.run()
>>> fitter_result = fitter_algo.getResult()
>>> gpr_algo = ot.GaussianProcessRegression(fitter_result)
>>> gpr_algo.run()
>>> gpr_result = gpr_algo.getResult()

Now, we define the mesh on which the gconditionned gaussian process  is generated:

>>> vertices = [[1.0, 0.0], [2.0, 0.0], [2.0, 1.0], [1.0, 1.0], [1.5, 0.5]]
>>> simplices = [[0, 1, 4], [1, 2, 4], [2, 3, 4], [3, 0, 4]]
>>> mesh2D = ot.Mesh(vertices, simplices)
>>> process = otexp.ConditionedGaussianProcess(gpr_result, mesh2D)

We get a realization of the conditioned Gaussian process:

>>> realization = process.getRealization()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ConditionedGaussianProcess::getRealization
"Return a realization of the process.

Returns
-------
realization : :class:`~openturns.Field`
    A realization of the process."
