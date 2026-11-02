%feature("docstring") OT::ChristoffelSubsampleExperiment
R"RAW(Christoffel subsample experiment.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Given an orthonormal basis of :math:`L^2(\cD, \mu)` and a target size
:math:`n`, the experiment builds the first :math:`m` basis functions and
returns a weighted design of :math:`n` points. The points are drawn from the
Christoffel distribution, or from the determinantal point process (DPP)
associated with its kernel, and the weights :math:`m / k_m` are the density
ratios that turn the design into a quadrature rule for :math:`\mu`. See
:any:`christoffel_sampling` for the theory of the Christoffel distribution,
of DPPs, and of the frame certificate.

Two sizing modes are available.

By default, :math:`m = n - k` is the largest dimension supplied by the basis:
:math:`k = 0` with an inexhaustible basis, and :math:`k = 1, 2, \dots` is
tried until the basis supplies :math:`m` functions. For :math:`n = m`, a
fresh design is an exact projection DPP of size :math:`m` associated with the
Christoffel kernel. For :math:`n > m`, the first :math:`m` points are a
projection DPP and the remaining :math:`n - m` points are independent draws
from the :class:`~openturns.experimental.ChristoffelDistribution`; the design
is then randomly permuted. See :any:`christoffel_dpp` for the exact
sequential DPP sampler and its cost.

When the ``ChristoffelSubsampleExperiment-Certified`` key is true, the
experiment uses the certified sizing of Cohen and Dolbeault
[cohendolbeault2020]_: :math:`m` is the largest integer with
:math:`n \ge \gamma m \log(m)`, where :math:`\gamma` is the
``ChristoffelSubsampleExperiment-Gamma`` key value, and a fresh design is
:math:`n` independent draws from the
:class:`~openturns.experimental.ChristoffelDistribution`. No thinning is
applied. The weighted design is checked against the frame bounds; if it
misses them, the whole independent sample is redrawn, up to
``ChristoffelSubsampleExperiment-MaxAttempts`` draws. This mode makes
:math:`m` much smaller than :math:`n` and is not the default; when the key is
false, :math:`m = n-k` is chosen by the basis capacity as described above.
See :any:`christoffel_frame` for the certified budget and the frame
condition.

When points are supplied to :meth:`generateWithWeights`, an empty sample of
any dimension selects the fresh path. Supplied candidates are assumed to be
draws from the reference measure. If more than :math:`n` points are supplied,
exactly :math:`n` of them are selected by exact volume sampling, with
probability proportional to the determinant of the Gram matrix of their basis
features; no fresh point is drawn. If exactly :math:`n` points are supplied,
all of them are kept with unit weights, since no Christoffel selection takes
place. If fewer than :math:`n` and at most :math:`m` points are supplied, all
of them are kept and the DPP is conditioned on their feature vectors; the
remaining :math:`m - M` DPP points are drawn, followed by
:math:`n - m` independent Christoffel points when :math:`n > m`.
Supplying more than :math:`m` points while asking for fewer than :math:`n`
points is rejected, as is a set of supplied features that is not linearly
independent. See :any:`christoffel_conditioning` for conditional DPPs and
volume sampling. In certified mode, supplied points are rejected because the
certificate assumes independent Christoffel draws; only the fresh path is
available.

Each design point of a fresh, conditional or volume-sampled design carries a
weight proportional to :math:`m / k_m(\vect{x}_i)`, the density ratio of the
reference measure over the Christoffel distribution. The exactly-:math:`n`
supplied design is the exception: it is returned unchanged with unit weights
because no selection takes place. The returned weights are positive and
finite: a point with a null or non-finite importance ratio is rejected instead
of being returned with a zero weight.

For a Christoffel-weighted design, the implementation checks the eigenvalues
of the normalized weighted design Gramian against the band
:math:`[1 - \tau, 1 + \tau]`, where :math:`\tau` is the
``ChristoffelSubsampleExperiment-FrameTolerance`` key, and warns when
they are missed. In the default mode this check is advisory: it is not a
certificate that the frame bounds hold. For :math:`n = m` the design is
square and the projection DPP has no certified frame guarantee; for
:math:`n > m` the extra independent Christoffel points improve the budget
without making the certificate exact. In certified mode, the independent
Christoffel budget :math:`n = \gamma m \log(m)` is the Cohen-Dolbeault
setting, so the frame guarantee holds with high probability under the
theorem assumptions; the realized design is checked and the draw is repeated
until it passes, up to the configured attempt limit. Use
:meth:`computeDesignEigenvalues` when a frame diagnostic is required on a
given design. See :any:`christoffel_frame` for the frame condition and the
certified budget.

Parameters
----------
basis : :class:`~openturns.OrthogonalBasis`
    Orthonormal basis defining the approximation space. Its embedded
    measure :math:`\mu` must be continuous.
size : positive int
    Number :math:`n` of points of the experiment.

See Also
--------
openturns.WeightedExperiment
openturns.experimental.ChristoffelDistribution

Notes
-----
A default-constructed experiment has no configured basis: call
:meth:`setOrthogonalBasis` (and optionally :meth:`setSize`) before
generating. Generation from the unconfigured default state throws.

The following :class:`~openturns.ResourceMap` keys are used:

- ``ChristoffelSubsampleExperiment-FrameTolerance`` (``Scalar``, default: ``0.5``): half-width of the accepted frame eigenvalue band around 1. It must be non-negative and finite.
- ``ChristoffelSubsampleExperiment-Certified`` (``Bool``, default: ``False``): when true, select the certified sizing :math:`n = \gamma m \log(m)` and independent Christoffel draws.
- ``ChristoffelSubsampleExperiment-Gamma`` (``Scalar``, default: ``9.242343873386666``): constant :math:`\gamma` of the certified sizing. It must be finite and strictly positive when the certified mode is selected.
- ``ChristoffelSubsampleExperiment-MaxAttempts`` (``UnsignedInteger``, default: ``10``): maximum number of independent certified draws tried before the last draw is returned with a frame warning.
- ``ChristoffelSubsampleExperiment-MaxProjectionAttempts`` (``UnsignedInteger``, default: ``1000000``): maximum number of Christoffel proposals tried to draw one admissible projection DPP point before an error is raised. It only guards against a pathological numerical failure.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> factory = ot.OrthogonalProductPolynomialFactory([ot.Uniform(-1.0, 1.0)])
>>> basis = ot.OrthogonalBasis(factory)
>>> experiment = otexp.ChristoffelSubsampleExperiment(basis, 60)
>>> print(experiment.getSpaceDimension())
60
>>> sample, weights = experiment.generateWithWeights()
>>> print(len(sample), len(weights))
60 60
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::getOrthogonalBasis
"Accessor to the orthogonal basis.

Returns
-------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis defining the approximation space."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::setOrthogonalBasis
R"RAW(Accessor to the orthogonal basis.

Parameters
----------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis defining the approximation space. Its embedded
    measure :math:`\mu` must be continuous.

Notes
-----
The space dimension and the Christoffel distribution are deduced
again from the new basis and the current size.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::getSpaceDimension
"Accessor to the space dimension deduced from the target size.

Returns
-------
spaceDimension : positive int
    Number of leading basis functions used."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::generateWithWeights
"Generate the weighted design.

Parameters
----------
suppliedSample : 2-d sequence of float, optional
    Supplied candidate points, assumed to be draws from the reference
    measure, e.g. already evaluated points to recycle. An empty sample,
    whatever its dimension, selects the fresh path. With more than ``size``
    points, exactly ``size`` of them are selected by exact volume sampling.
    With exactly ``size`` points, all of them are returned unchanged with
    unit weights. With fewer than ``size`` and at most ``spaceDimension``
    points, all of them are kept and the design is completed by a conditional
    projection DPP, plus independent Christoffel points when ``size`` exceeds
    the space dimension. More than ``spaceDimension`` supplied points while
    asking for fewer than ``size`` points is rejected. In certified mode, a
    non-empty supplied sample is rejected: only independent Christoffel
    draws carry the certificate.

Returns
-------
sample : :class:`~openturns.Sample`
    The design of ``size`` points.
weights : :class:`~openturns.Point`
    The associated importance weights."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::computeDesignEigenvalues
"Eigenvalues of the weighted design Gramian.

Parameters
----------
sample : 2-d sequence of float
    Design points.
weights : sequence of float
    Positive finite weight of each design point.

Returns
-------
eigenvalues : :class:`~openturns.Point`
    Eigenvalues of the mean over the design of weight times outer
    product, the frame stability certificate.

Notes
-----
The weights must be positive and finite; a zero-weight design point is
rejected because the frame certificate would silently ignore it."

// ---------------------------------------------------------------------

%feature("docstring") OT::ChristoffelSubsampleExperiment::DeduceSpaceDimension
R"RAW(Deduce the space dimension from a target size.

In the default mode the dimension is the target size
(:math:`k = 0` in :math:`m = n - k`). With a finite basis, the
constructor builds functions until the basis is exhausted and keeps
the largest available dimension. With :math:`m = n` the projection DPP
is sampled sequentially, so keep the target size moderate.

When ``ChristoffelSubsampleExperiment-Certified`` is true, the result is
the largest :math:`m \ge 1` such that
:math:`\gamma m \log(m) \le n`, with :math:`\gamma` read from
``ChristoffelSubsampleExperiment-Gamma``. The basis capacity is still
applied by the constructor.

Parameters
----------
size : positive int
    Target number of points.

Returns
-------
spaceDimension : positive int
    The target size in the default mode, or the certified dimension
    :math:`m` when the certified mode is selected.
)RAW"
