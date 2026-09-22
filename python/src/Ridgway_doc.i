%define OT_Ridgway_doc
R"RAW(Base class.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``Ridgway-LowDiscrepancySequence`` (``String``, default: ``SobolSequence``): name of the low discrepancy sequence used to compute the integral; the possible values are the names of the classes derived from :class:`~openturns.LowDiscrepancySequence`, such as ``SobolSequence``, ``HaltonSequence``, ``ReverseHaltonSequence``, ``FaureSequence`` or ``HaselgroveSequence``.
- ``Ridgway-DefaultAlpha`` (``Scalar``, default: ``0.5``): value of the alpha parameter.
- ``Ridgway-DefaultStudentSampleSize`` (``UnsignedInteger``, default: ``1024``): size of the sample used to compute the Student distribution.
- ``Ridgway-DefaultParticleNumber`` (``UnsignedInteger``, default: ``1000``): number of particles.
)RAW"
%enddef
