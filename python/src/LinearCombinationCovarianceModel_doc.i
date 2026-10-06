%feature("docstring") OT::LinearCombinationCovarianceModel
R"RAW(Linear combination of scalar covariance models.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The model writes, for all :math:`(\vect{s}, \vect{t})`,

.. math::

    C(\vect{s}, \vect{t}) = \left(\sum_{k=1}^K \alpha_k C_k(\vect{s}, \vect{t})\right) R

where the :math:`C_k` are scalar covariance models, the :math:`\alpha_k` are
nonnegative variance weights and :math:`R` is the shared output correlation
matrix. The amplitude of an atom is the square root of its variance weight.
All the atoms must share the same input dimension; they are scalar-output
models. For an output dimension :math:`p>1`, all the output variances are
equal to :math:`\sum_k \alpha_k C_k(\vect{s}, \vect{s})`.

A legacy atom with amplitude :math:`\sigma` and nugget factor :math:`\tau` is
normalized when it enters the combination: its variance
:math:`\sigma^2` is absorbed into the coefficient and its nugget
:math:`\tau\sigma^2` is represented by a unit
:class:`~openturns.DiracCovarianceModel` atom. The generic amplitude and
nugget parameters of the atoms are therefore frozen, while their other
parameters remain available for estimation.

The coefficients are the first entries of the full parameter, followed by
the full parameters of the atoms and by the lower triangle of the output
correlation matrix.

Available constructors:
    LinearCombinationCovarianceModel()

    LinearCombinationCovarianceModel(atoms, coefficients, outputDimension=1)

Parameters
----------
atoms : sequence of :class:`~openturns.CovarianceModel`
    The scalar covariance models. Nested scalar combinations are flattened
    and identical atoms are folded.
coefficients : sequence of float
    The nonnegative variance weights, one per input atom. When atoms are
    folded, their weights are summed.
outputDimension : int
    The output dimension :math:`p\geq 1`.

See Also
--------
:class:`~openturns.CovarianceModel`,
:class:`~openturns.DiracCovarianceModel`,
:class:`~openturns.ProductCovarianceModel`

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> smooth = ot.MaternModel([1.0], [2.0], 2.5)
>>> noise = ot.DiracCovarianceModel(1, [0.5])
>>> model = otexp.LinearCombinationCovarianceModel([smooth, noise], [1.0, 1.0])
>>> model.getOutputDimension()
1
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::getAtoms
R"RAW(Accessor to the scalar atoms, after normalization and folding.

Returns
-------
atoms : sequence of :class:`~openturns.CovarianceModel`
    The scalar atoms.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::setAtoms
R"RAW(Replace the scalar atoms.

The coefficients are kept, so their number must match the number of input
atoms. The output correlation is preserved. The managed nugget state is
reset: the coefficients are attached to the new atoms as ordinary variance
weights.

Parameters
----------
atoms : sequence of :class:`~openturns.CovarianceModel`
    The scalar atoms.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::getCoefficients
R"RAW(Accessor to the variance weights.

Returns
-------
coefficients : :class:`~openturns.Point`
    The nonnegative variance weights.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::setCoefficients
R"RAW(Set the variance weights.

Parameters
----------
coefficients : sequence of float
    The nonnegative variance weights, one per atom.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::getAmplitudes
R"RAW(Accessor to the atom amplitudes.

Returns
-------
amplitudes : :class:`~openturns.Point`
    The square roots of the variance weights.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::setAmplitudes
R"RAW(Set the atom amplitudes.

Parameters
----------
amplitudes : sequence of float
    The nonnegative amplitudes, one per atom.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::getScale
R"RAW(Generic scale accessor.

A linear combination has no unique scale: this method raises an exception.
Use :meth:`getAtoms` or the full parameter accessors instead.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::setScale
R"RAW(Generic scale accessor.

A linear combination has no unique scale: this method raises an exception.
Use :meth:`setAtoms` or the full parameter accessors instead.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::getAmplitude
R"RAW(Accessor to the total standard deviation of the non-managed atoms.

For an output dimension :math:`p>1`, all the components are equal.

Returns
-------
amplitude : :class:`~openturns.Point`
    The standard deviations.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::setAmplitude
R"RAW(Set the total standard deviation of the non-managed atoms.

The variance weights of the non-managed atoms are scaled to the requested
variance. A managed nugget factor is preserved.

Parameters
----------
amplitude : sequence of float
    The standard deviations: one value or :math:`p` equal values.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::getNuggetFactor
R"RAW(Accessor to the managed nugget factor.

Returns
-------
nuggetFactor : float
    The nugget variance divided by the total variance of the non-managed
    atoms, or 0 if no managed nugget has been created.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::setNuggetFactor
R"RAW(Set the managed nugget factor.

The nugget variance is the nugget factor times the total variance of the
non-managed atoms. The corresponding unit Dirac atom is created on first
use and its coefficient is inactive by default; use
:meth:`activateNuggetFactor` to estimate it.

Parameters
----------
nuggetFactor : float
    The nonnegative nugget factor.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::getNuggetVariance
R"RAW(Accessor to the managed nugget variance.

Returns
-------
nuggetVariance : float
    The variance of the managed nugget, or 0 if none has been created.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::setNuggetVariance
R"RAW(Set the managed nugget variance.

The corresponding unit Dirac atom is created on first use and its coefficient
is inactive by default; use :meth:`activateNuggetFactor` to estimate it.

Parameters
----------
nuggetVariance : float
    The nonnegative nugget variance.
)RAW"

%feature("docstring") OT::LinearCombinationCovarianceModel::activateNuggetFactor
R"RAW(Activate or deactivate the managed nugget coefficient.

Parameters
----------
isNuggetFactorActive : bool
    Whether the managed nugget coefficient belongs to the active parameters.
)RAW"
