//                                               -*- C++ -*-
/**
 *  @file SymbolicEvaluation_doc.i
 *
 *  Docstrings for SymbolicEvaluation
 */

%feature("docstring") OT::SymbolicEvaluation
R"RAW(Evaluation of an analytical function defined by symbolic formulas.

This class implements an :class:`~openturns.EvaluationImplementation` based on a
symbolic parser. It evaluates one or several analytical formulas given as strings.

Available constructors:
    SymbolicEvaluation()

    SymbolicEvaluation(*inputVariablesNames, outputVariablesNames, formulas*)

    SymbolicEvaluation(*inputVariablesNames, outputVariablesNames, formula*)

Parameters
----------
inputVariablesNames : sequence of `str`
    Names of the input variables used in the formula(s).
outputVariablesNames : sequence of `str`
    Names of the output variables associated with the formula(s).
formulas : sequence of `str`
    List of formulas, one per output component. Its length must match the output
    dimension.
formula : str
    Single formula.

Notes
-----
This class distinguishes two naming layers:

* The *input/output descriptions* inherited from
  :class:`~openturns.EvaluationImplementation`, used as user-level labels.

* The *input/output variables names* used internally by the symbolic parser in the
  formulas.

These two layers may differ.

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
[3,2]

Inspect parser variable names:

>>> ev.getInputVariablesNames()
[x0,x1]
>>> ev.getOutputVariablesNames()
[y0,y1]
>>> ev.getFormulas()
[x0 + x1,x0 * x1]
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::getInputVariablesNames
R"RAW(Accessor to input variables names used by the symbolic parser.

These names are the internal parser symbols used in the formulas.
They may differ from the input description returned by :meth:`getInputDescription`.

Returns
-------
inputVariablesNames : sequence of `str`
    Input variable names used by the parser.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::getOutputVariablesNames
R"RAW(Accessor to output variables names used by the symbolic parser.

These names are the internal parser symbols used in the formulas.
They may differ from the output description returned by
:meth:`getOutputDescription`.

Returns
-------
outputVariablesNames : sequence of `str`
    Output variable names used by the parser.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SymbolicEvaluation::getFormulas
R"RAW(Accessor to the symbolic formulas.

Returns
-------
formulas : sequence of `str`
    The formulas, one per output component.
)RAW"
