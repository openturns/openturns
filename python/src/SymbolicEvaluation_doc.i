%feature("docstring") OT::SymbolicEvaluation
"Evaluation of an analytical function defined by symbolic formulas.

This class implements an :class:`~openturns.EvaluationImplementation` based on a
symbolic parser. It evaluates one or several analytical formulas given as strings.

Available constructors:
    SymbolicEvaluation()

    SymbolicEvaluation(*inputVariablesNames, outputVariablesNames, formulas*)

    SymbolicEvaluation(*inputVariablesNames, outputVariablesNames, formula*)

Parameters
----------
inputVariablesNames : sequence of `str`
    Names of the input variables used in the formula(s) (parser variable names).
outputVariablesNames : sequence of `str`
    Names of the output variables used in the formula(s) (parser variable names).
formulas : sequence of `str`
    List of formulas, one per output component. Its length must match the output
    dimension.
formula : str
    Single formula.

Notes
-----
This class maintains two different naming layers:

* The *input/output descriptions* (handled by :class:`~openturns.EvaluationImplementation`)
  are user-level labels (e.g. for display, reporting, downstream processing).
  They are accessed with :meth:`getInputDescription` / :meth:`getOutputDescription`
  and can be modified with :meth:`setInputDescription` / :meth:`setOutputDescription`.

* The *input/output variables names* are internal names used by the symbolic parser
  (ExprTk) to bind variables in the formulas. They are accessed with
  :meth:`getInputVariablesNames` and :meth:`getOutputVariablesNames`.

These two layers may differ. In particular, changing the output description does not
necessarily change the internal parser variable names, because formulas must remain
consistent with the symbols expected by ExprTk.

The overridden :meth:`setInputDescription` and :meth:`setOutputDescription` implement
a non-regressive synchronization rule: internal parser names are updated from the new
description only if they were still equal to the previous description. This prevents
overwriting user-provided parser names.

See also
--------
SymbolicFunction, Evaluation, EvaluationImplementation

Examples
--------
Define and evaluate a multi-output symbolic evaluation:

>>> import openturns as ot
>>> inNames = ['x0', 'x1']
>>> outNames = ['y0', 'y1']
>>> formulas = ['x0 + x1', 'x0 * x1']
>>> ev = ot.SymbolicEvaluation(inNames, outNames, formulas)
>>> ev([1.0, 2.0])

Example illustrating the difference between output description and parser variable names:

>>> import openturns as ot
>>> f = ot.SymbolicFunction(['x1', 'x2'], ['x1 + x2', 'x1 - x2'])
>>> ev = f.getEvaluation().getImplementation()
>>> ev.setOutputDescription(['f1', 'f2'])
>>> ev.getOutputDescription()
>>> ev.getOutputVariablesNames()
>>> ev.getFormulas()
"

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::clone
"Virtual constructor.

Returns
-------
clone : :class:`~openturns.SymbolicEvaluation`
    A copy of the object."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::operator==
"Comparison operator."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::__repr__
"String representation of the object."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::__str__
"Pretty string representation of the object."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::_repr_html_
"HTML representation of the object (for notebooks)."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::operator()
"Evaluate the symbolic formulas.

Parameters
----------
inP : sequence of `float`
    Input point.
inS : 2-d sequence of `float`
    Input sample.

Returns
-------
outP : :class:`~openturns.Point`
    Output point.
outS : :class:`~openturns.Sample`
    Output sample."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::getInputDimension
"Accessor for input dimension.

Returns
-------
dim : int
    Input dimension."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::getOutputDimension
"Accessor for output dimension.

Returns
-------
dim : int
    Output dimension."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::getMarginal
"Get a marginal evaluation.

Parameters
----------
i : int
    Index of the output component.
indices : sequence of `int`
    Indices of the output components.

Returns
-------
marginal : :class:`~openturns.Evaluation`
    The corresponding marginal evaluation."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::getInputVariablesNames
"Accessor to input variables names used by the symbolic parser.

These names are the internal parser symbols used to bind variables in the formulas.
They may differ from the input description returned by :meth:`getInputDescription`.

Returns
-------
inputVariablesNames : sequence of `str`
    Input variable names used by the parser."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::getOutputVariablesNames
"Accessor to output variables names used by the symbolic parser.

These names are internal parser symbols and may differ from the output description
returned by :meth:`getOutputDescription`. Changing the output description does not
necessarily change these internal names, because formulas must remain consistent with
the parser symbols.

Returns
-------
outputVariablesNames : sequence of `str`
    Output variable names used by the parser."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::getFormulas
"Accessor to the symbolic formulas.

Returns
-------
formulas : sequence of `str`
    The formulas, one per output component, expressed using the parser variable names."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::setInputDescription
"Set the input description.

This method updates the base class input description. It also applies a
non-regressive synchronization rule between the base description and the internal
parser variable names: internal names are updated only if they were still equal to
the previous description."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::setOutputDescription
"Set the output description.

This method updates the base class output description. It also applies a
non-regressive synchronization rule between the base description and the internal
parser variable names: internal names are updated only if they were still equal to
the previous description."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::isLinear
"Linearity accessor.

Returns
-------
isLinear : bool
    Whether the evaluation is linear."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::isLinearlyDependent
"Check whether an output component is linearly dependent.

Parameters
----------
index : int
    Index of the output component.

Returns
-------
isDependent : bool
    Whether the specified component is linearly dependent."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::isParallel
"Whether it is safe to call in parallel.

Returns
-------
isParallel : bool
    Whether the evaluation can be called in parallel."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::setCheckOutput
"Enable/disable invalid values check on the output.

Parameters
----------
checkOutput : bool
    Whether to check invalid values in the output."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::save
"Store the object through the StorageManager."

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::load
"Reload the object from the StorageManager."
