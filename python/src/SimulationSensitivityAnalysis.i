// SWIG file SimulationSensitivityAnalysis.i

%{
#include "openturns/SimulationSensitivityAnalysis.hxx"
%}

%include SimulationSensitivityAnalysis_doc.i

%include openturns/SimulationSensitivityAnalysis.hxx
namespace OT{ %extend SimulationSensitivityAnalysis { SimulationSensitivityAnalysis(const SimulationSensitivityAnalysis & other) { return new OT::SimulationSensitivityAnalysis(other); } } }
