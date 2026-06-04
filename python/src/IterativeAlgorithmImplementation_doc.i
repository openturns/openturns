%define OT_IterativeAlgorithm_doc
"
Iterative algorithm

Parameters
----------
dimension : int, default=1
    Dimension of the input data.

Notes
-----
Base class for iterative algorithms: persistent objects that can be incremented iteratively.
"
%enddef

%feature("docstring") OT::IterativeAlgorithmImplementation
OT_IterativeAlgorithm_doc

// ---------------------------------------------------------------------

%define OT_IterativeAlgorithm_increment_doc
"Increment the internal data.

Parameters
----------
data : sequence of float or 2-d sequence of float
    New input point or sample."
%enddef

%feature("docstring") OT::IterativeAlgorithmImplementation::increment
OT_IterativeAlgorithm_increment_doc

%define OT_IterativeAlgorithm_getIterationNumber_doc
"Get the current iteration of the algorithm

Returns
-------
iteration : int
    Current iteration of the algorithm"
%enddef

%feature("docstring") OT::IterativeAlgorithmImplementation::getIterationNumber
OT_IterativeAlgorithm_getIterationNumber_doc

%define OT_IterativeAlgorithm_getDimension_doc
"Get the dimension of the algorithm

Returns
-------
dimension : int
    Dimension of the algorithm"
%enddef

%feature("docstring") OT::IterativeAlgorithmImplementation::getDimension
OT_IterativeAlgorithm_getDimension_doc
