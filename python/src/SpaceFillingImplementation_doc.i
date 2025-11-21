%define OT_SpaceFilling_doc
"Space filling computation base class.

Notes
-----
Compute a space filling criterion for a design, e.g. a metric that underlines
how points fill the uniform space."

%enddef
%feature("docstring") OT::SpaceFillingImplementation
OT_SpaceFilling_doc
// ---------------------------------------------------------------------
%define OT_SpaceFilling_evaluate_doc
"Compute the criterion for a specific design.

Parameters
----------
design : :class:`~openturns.Sample` or 2d array like
    The design

Returns
-------
value:  float
    The computed criterion"
%enddef
%feature("docstring") OT::SpaceFillingImplementation::evaluate
OT_SpaceFilling_evaluate_doc

// ---------------------------------------------------------------------

%define OT_SpaceFilling_isMinimizationProblem
"Minimization flag accessor.

Returns
-------
isMinimization : bool
    Whether the problem is a minimization.
"
%enddef
%feature("docstring") OT::SpaceFillingImplementation::isMinimizationProblem
OT_SpaceFilling_isMinimizationProblem

// ---------------------------------------------------------------------

%define OT_SpaceFilling_perturbLHS
"Elementary perturbation.

Parameters
----------
design : :class:`~openturns.Sample`
    The design to perturb (in-place)
oldCriterion : float
    The previous value of the criterion
row1 : int
    First row index
row2 : int
    Second row index

Returns
-------
criterion : float
    The value of the criterion"
%enddef
%feature("docstring") OT::SpaceFillingImplementation::perturbLHS
OT_SpaceFilling_perturbLHS
