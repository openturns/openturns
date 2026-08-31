
%feature("docstring") OT::ParametricEvaluation
"Evaluation with fixed parameters.

Parameters
----------
evaluation : :class:`~openturns.NumericalMathEvaluationImplementation`
    The evaluation.
inputPositions : :class:`~openturns.Indices`
    The positions of the parameters in the input vector.
parameters : :class:`~openturns.NumericalPoint`
    The parameter values."



%feature("docstring") OT::ParametricEvaluation::getFunction
"Accessor to the underlying function.

Returns
-------
evaluation : :class:`~openturns.NumericalMathEvaluationImplementation`
    The underlying evaluation"



%feature("docstring") OT::ParametricEvaluation::getInputPositions
"Accessor to the input positions.

Returns
-------
positions : :class:`~openturns.Indices`
    The positions of the parameters in the input vector"



%feature("docstring") OT::ParametricEvaluation::getParametersPositions
"Accessor to the parameters positions.

Returns
-------
positions : :class:`~openturns.Indices`
    The positions of the parameters in the input vector"

