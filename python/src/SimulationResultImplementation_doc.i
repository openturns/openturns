%define OT_SimulationResultImplementation_doc
"EventSimulation result base class.

Notes
-----
Structure created by the method run() of a :class:`~openturns.EventSimulation`,
and obtained thanks to the method getResult()."
%enddef
%feature("docstring") OT::SimulationResultImplementation
OT_SimulationResultImplementation_doc

// ---------------------------------------------------------------------

%define OT_SimulationResultImplementation_getBlockSize_doc
"Accessor to the block size.

Returns
-------
blockSize : int
    Number of terms in the probability simulation estimator grouped together."
%enddef
%feature("docstring") OT::SimulationResultImplementation::getBlockSize
OT_SimulationResultImplementation_getBlockSize_doc

// ---------------------------------------------------------------------

%define OT_SimulationResultImplementation_setBlockSize_doc
R"RAW(Accessor to the block size.

Parameters
----------
blockSize : int, :math:`blockSize \geq 0`
    Number of terms in the probability simulation estimator grouped together.)RAW"
%enddef
%feature("docstring") OT::SimulationResultImplementation::setBlockSize
OT_SimulationResultImplementation_setBlockSize_doc

// ---------------------------------------------------------------------

%define OT_SimulationResultImplementation_getOuterSampling_doc
"Accessor to the outer sampling.

Returns
-------
outerSampling : int
    Number of groups of terms in the probability simulation estimator."
%enddef
%feature("docstring") OT::SimulationResultImplementation::getOuterSampling
OT_SimulationResultImplementation_getOuterSampling_doc

// ---------------------------------------------------------------------

%define OT_SimulationResultImplementation_setOuterSampling_doc
R"RAW(Accessor to the outer sampling.

Parameters
----------
outerSampling : int, :math:`outerSampling \geq 0`
    Number of groups of terms in the probability simulation estimator.)RAW"
%enddef
%feature("docstring") OT::SimulationResultImplementation::setOuterSampling
OT_SimulationResultImplementation_setOuterSampling_doc

// ---------------------------------------------------------------------

%define OT_SimulationResultImplementation_setTimeDuration_doc
"Accessor to the elapsed time.

Parameters
----------
time : float
    Simulation duration in seconds"
%enddef
%feature("docstring") OT::SimulationResultImplementation::setTimeDuration
OT_SimulationResultImplementation_setTimeDuration_doc

// ---------------------------------------------------------------------

%define OT_SimulationResultImplementation_getTimeDuration_doc
"Accessor to the elapsed time.

Returns
-------
time : float
    Simulation duration in seconds"
%enddef
%feature("docstring") OT::SimulationResultImplementation::getTimeDuration
OT_SimulationResultImplementation_getTimeDuration_doc
