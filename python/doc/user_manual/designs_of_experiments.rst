======================
Designs of experiments
======================

.. currentmodule:: openturns

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Experiment

Stratified designs of experiments
=================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    StratifiedExperiment

.. autosummary::
    :toctree: _generated/
    :template: StratifiedExperiment.rst_t

    Axial
    Factorial
    Composite

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    Box

Weighted experiments
====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    WeightedExperiment

Random weighted experiments
---------------------------

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    BootstrapExperiment
    LHSExperiment
    MonteCarloExperiment
    ImportanceSamplingExperiment

Deterministic weighted experiments
----------------------------------

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    FixedExperiment
    GaussProductExperiment
    LowDiscrepancyExperiment

.. _low_discrepancy_sequence_api:

Low Discrepancy Sequences
=========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    LowDiscrepancySequence

.. autosummary::
    :toctree: _generated/
    :template: LowDiscrepancySequence.rst_t

    FaureSequence
    HaltonSequence
    ReverseHaltonSequence
    HaselgroveSequence
    SobolSequence

Optimal LHS generation
----------------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    OptimalLHSExperiment
    MonteCarloLHS
    SimulatedAnnealingLHS
    LHSResult

Space Filling
~~~~~~~~~~~~~
Used to define the optimization criterion in :class:`~openturns.OptimalLHSExperiment`.

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SpaceFilling
    SpaceFillingC2
    SpaceFillingMinDist
    SpaceFillingPhiP

Temperature Profile
~~~~~~~~~~~~~~~~~~~
Used to control the optimization in :class:`~openturns.SimulatedAnnealingLHS`.

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    TemperatureProfile
    LinearProfile
    GeometricProfile
