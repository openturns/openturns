// SWIG file SensitivityAnalysis.i

%{
#include "openturns/SensitivityAnalysis.hxx"
%}

%include SensitivityAnalysis_doc.i

%include openturns/SensitivityAnalysis.hxx
namespace OT{ %extend SensitivityAnalysis { SensitivityAnalysis(const SensitivityAnalysis & other) { return new OT::SensitivityAnalysis(other); } } }
