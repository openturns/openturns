%feature("docstring") OT::LeaveOneOut
R"RAW(Leave-one-out model selection score.

The score is the weight-mass normalized PRESS statistic (Allen, 1974):
with the weighted least-squares fit :math:`\hat{\vect{y}}`, the residual
:math:`r_i = y_i - \hat{y}_i` and the weighted leverage
:math:`h_i = w_i \psi(\vect{u}_i)^\intercal \mat{G}^{-1} \psi(\vect{u}_i)`
with :math:`\mat{G} = \mat{\Psi}^\intercal \mat{W} \mat{\Psi}` returned by
:meth:`~openturns.LeastSquaresMethod.getHDiag`,

.. math::
    \mathrm{Err}_{\mathrm{LOO}} =
    \frac{1}{\sum_{i=1}^n w_i} \sum_{i=1}^n
    w_i \left(\frac{r_i}{1 - h_i}\right)^2,

relative to the weight-mass normalized output variance
:math:`\sum w_i (y_i - \bar{y}_w)^2 / \sum w_i` around the weighted mean.
Dropping an observation is a rank-one downdate of the weighted normal
equations, so the score is exact for any weights, uniform or not.
Unlike :class:`~openturns.CorrectedLeaveOneOut`, no small-sample
correction factor is applied.

See also
--------
FittingAlgorithm, KFold, CorrectedLeaveOneOut

Notes
-----
LeaveOneOut inherits from :class:`~openturns.FittingAlgorithm`.

This class is not usable on its own is because it has sense only within the
:class:`~openturns.FunctionalChaosAlgorithm`.)RAW"
