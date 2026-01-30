%feature("docstring") OT::PointToPointConnection
R"RAW(Composition function :math:`f \circ g` where :math:`g` acts on points and :math:`f` produces points.

Available constructors:
   PointToPointEvaluation(*f_Function, g_Function*)

   PointToPointEvaluation(*f_FieldToPoint, g_PointToField*)

Parameters
----------
f_Function, g_Function :  :class:`~openturns.Function`
    Function that acts on points and produces points
f_FieldToPoint : :class:`~openturns.FieldToPointFunction`
    Function that acts on fields and produces points
g_PointToField : :class:`~openturns.PointToFieldFunction`
    Function that acts on points and produces fields

Notes
-----
This class is a particular case  of the general class :class:`~openturns.Function`. Thus it implements the evaluation operator and the evaluation of the gradient and the hessian.

By default, the gradient and hessian are evaluated using  centered finite differences described in :class:`~openturns.CenteredFiniteDifferenceGradient` and :class:`~openturns.CenteredFiniteDifferenceHessian`.)RAW"

