// SWIG file SensitivityAnalysis.i

%{
#include "SensitivityAnalysis.hxx"
%}

%include SensitivityAnalysis_doc.i

%include SensitivityAnalysis.hxx
namespace OT{ %extend SensitivityAnalysis { SensitivityAnalysis(const SensitivityAnalysis & other) { return new OT::SensitivityAnalysis(other); } } }
