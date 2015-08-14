// SWIG file SimulationSensitivityAnalysis.i

%{
#include "SimulationSensitivityAnalysis.hxx"
%}

%include SimulationSensitivityAnalysis_doc.i

%include SimulationSensitivityAnalysis.hxx
namespace OT{ %extend SimulationSensitivityAnalysis { SimulationSensitivityAnalysis(const SimulationSensitivityAnalysis & other) { return new OT::SimulationSensitivityAnalysis(other); } } }
