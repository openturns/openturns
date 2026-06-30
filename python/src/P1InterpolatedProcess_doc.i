%feature("docstring") OT::P1InterpolatedProcess
R"RAW(P1-interpolated process.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructor:
    P1InterpolatedProcess(*compoundProcess, mesh*)

Parameters
----------
compoundProcess : :class:`~openturns.Process`
    The process whose realizations are interpolated.
mesh : :class:`~openturns.Mesh`
    The mesh on which the interpolation is computed.

Notes
-----
This class builds a new process by applying a P1 (piecewise-linear) Lagrange
interpolation to the realizations of a given process on a target mesh.

Let :math:`X: \Omega \times \cD \mapsto \Rset^d` be the compound process
defined on the mesh :math:`\cM_X` and let :math:`\cM` be the target mesh.
For any realization :math:`x(\cdot)` of :math:`X`, the corresponding
realization of the interpolated process :math:`Y` on :math:`\cM` is:

.. math::

    Y(\omega, \vect{t}) = \Pi_{\cM_X \to \cM}(x(\omega, \cdot))(\vect{t})

where :math:`\Pi` denotes the P1 Lagrange interpolation operator from
:math:`\cM_X` to :math:`\cM`.

The P1 Lagrange interpolation is performed by the
:class:`~openturns.P1LagrangeInterpolation` class.

Examples
--------
Create a Gaussian process on a coarse mesh and interpolate its realizations
on a finer mesh:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> amplitude = [1.0]
>>> scale = [1.0]
>>> covarianceModel = ot.ExponentialModel(scale, amplitude)
>>> coarseMesh = ot.RegularGrid(0.0, 0.5, 11)
>>> gaussianProcess = ot.GaussianProcess(covarianceModel, coarseMesh)
>>> fineMesh = ot.RegularGrid(0.0, 0.1, 51)
>>> process = otexp.P1InterpolatedProcess(gaussianProcess, fineMesh)
>>> realization = process.getRealization()
>>> print(realization.getOutputDimension())
1
>>> print(realization.getMesh().getVerticesNumber())
51

)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::P1InterpolatedProcess::getCompoundProcess
"Get the compound process.

Returns
-------
compoundProcess : :class:`~openturns.Process`
    The process whose realizations are interpolated."

// ---------------------------------------------------------------------

%feature("docstring") OT::P1InterpolatedProcess::setMesh
"Set the mesh.

Parameters
----------
mesh : :class:`~openturns.Mesh`
    The mesh on which the interpolation is computed."
