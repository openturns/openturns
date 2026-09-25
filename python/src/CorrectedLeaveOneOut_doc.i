%feature("docstring") OT::CorrectedLeaveOneOut
R"RAW(Corrected leave-one-out model selection score for uniform weights.

This score is only defined for uniform weights: any other weight
pattern raises an exception, use :class:`~openturns.LeaveOneOut` or
:class:`~openturns.KFold` for weighted designs.

The leave-one-out error is the PRESS statistic (Allen, 1974): with the
least-squares fit :math:`\hat{\vect{y}}`, the residual
:math:`r_i = y_i - \hat{y}_i` and the leverage
:math:`h_i = \psi(\vect{u}_i)^\intercal (\mat{\Psi}^\intercal \mat{\Psi})^{-1} \psi(\vect{u}_i)`
returned by :meth:`~openturns.LeastSquaresMethod.getHDiag`,

.. math::
    \mathrm{Err}_{\mathrm{LOO}} =
    \frac{1}{n} \sum_{i=1}^n
    \left(\frac{r_i}{1 - h_i}\right)^2,

relative to the unbiased output sample variance. It is multiplied by
the small-sample correction factor
:math:`n/(n-p)\,(1+\mathrm{Tr}(\mat{C}^{-1})/n)` of [chapelle2002]_ used
in [blatman2011]_, with
:math:`\mat{C} = \mat{\Psi}^\intercal \mat{\Psi} / n`. This correction
has no extension to non-uniform weights, hence the uniform-only
restriction.

See also
--------
FittingAlgorithm, KFold, LeaveOneOut

Notes
-----
CorrectedLeaveOneOut inherits from :class:`~openturns.FittingAlgorithm`.

This class is not usable on its own is because it has sense only within the
:class:`~openturns.FunctionalChaosAlgorithm`.)RAW"
