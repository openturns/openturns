%define OT_SequentialSamplingAlgorithm_doc
"Approximation algorithm.

Notes
-----
This class is not usable because it has sense only within the
:class:`~openturns.FunctionalChaosAlgorithm`."
%enddef
%feature("docstring") OT::SequentialSamplingAlgorithmImplementation
OT_SequentialSamplingAlgorithm_doc

// ---------------------------------------------------------------------

%define OT_SequentialSamplingAlgorithm_getInputSample_doc
"Accessor to the input sample.

Returns
-------
x : :class:`~openturns.Sample`
    Input sample"
%enddef
%feature("docstring") OT::SequentialSamplingAlgorithmImplementation::getInputSample
OT_SequentialSamplingAlgorithm_getInputSample_doc

// ---------------------------------------------------------------------

%define OT_SequentialSamplingAlgorithm_getOutputSample_doc
"Accessor to the output sample.

Returns
-------
y : :class:`~openturns.Sample`
    Input sample"
%enddef
%feature("docstring") OT::SequentialSamplingAlgorithmImplementation::getOutputSample
OT_SequentialSamplingAlgorithm_getOutputSample_doc

// ---------------------------------------------------------------------

%define OT_SequentialSamplingAlgorithm_getGenerationIndices_doc
"Accessor to the generation indices.

Returns
-------
generationIndices : :class:`~openturns.Indices`
    Indices of last element for each generation.
    This is updated each time the `update()` method is evaluated."
%enddef
%feature("docstring") OT::SequentialSamplingAlgorithmImplementation::getGenerationIndices
OT_SequentialSamplingAlgorithm_getGenerationIndices_doc

// ---------------------------------------------------------------------

%define OT_SequentialSamplingAlgorithm_generate_doc
"Generate a new sample.

Parameters
----------
size : int
    Size of sample to generate

Returns
-------
x : :class:`~openturns.Sample`
    New input sample.
"
%enddef
%feature("docstring") OT::SequentialSamplingAlgorithmImplementation::generate
OT_SequentialSamplingAlgorithm_generate_doc

// ---------------------------------------------------------------------

%define OT_SequentialSamplingAlgorithm_update_doc
"Update the current sample.

Adds a new incremental sample (input and output) to the existing sample.

Parameters
----------
x, y : :class:`~openturns.Sample`
    New incremental sample.
"
%enddef
%feature("docstring") OT::SequentialSamplingAlgorithmImplementation::update
OT_SequentialSamplingAlgorithm_update_doc
