%define OT_SpaceFillingPhiP_doc
R"RAW(Space filling PhiP.

See also
--------
SpaceFilling, SpaceFillingMinDist, SpaceFillingC2

Parameters
----------
p : int (default=50)
    Parameter for parametrizing the criterion

Notes
-----
Compute the criterion based a sample points denoted by :math:`(\vect{x}_1, \dots, \vect{x}_\sampleSize)` where :math:`\vect{x}_k \in \Rset^\inputDim`:

.. math::
    :nowrap:

    \begin{equation*}
      \phi_p((\vect{x}_1, \dots, \vect{x}_\sampleSize)) = \left(\sum_{i=1}^{\sampleSize} \sum_{j<i}
      \|\vect{x}_i - \vect{x}_j\|_2^{-p} \right)^{\frac{1}{p}}
    \end{equation*}

This criterion is known to converge to the MinDist criterion defined in :class:`~openturns.SpaceFillingMinDist` (which is the minimal distance between two distinct elements of the design) criterion when :math:`p` tends to infinity.
)RAW"

%enddef
%feature("docstring") OT::SpaceFillingPhiP
OT_SpaceFillingPhiP_doc
// ---------------------------------------------------------------------
%define OT_SpaceFillingPhiP_evaluate_doc
"Compute the PhiP criterion for a specific design.

Parameters
----------
design : :class:`~openturns.Sample` or 2d array like
    The design

Returns
-------
crit : float
    The phiP criterion

Examples
--------
>>> import openturns as ot
>>> # Build an LHS using openturns class
>>> lhs = ot.LHSExperiment(ot.Uniform(), 100)
>>> design = lhs.generate()
>>> # Compute the PhiP criterion
>>> crit = ot.SpaceFillingPhiP(50).evaluate(design)"
%enddef
%feature("docstring") OT::SpaceFillingPhiP::evaluate
OT_SpaceFillingPhiP_evaluate_doc
// ---------------------------------------------------------------------
