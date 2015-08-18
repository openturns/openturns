// SWIG file QuasiMonteCarloResult.i

%{
#include "QuasiMonteCarloResult.hxx"
%}

%include QuasiMonteCarloResult_doc.i

%include QuasiMonteCarloResult.hxx
namespace OT{ %extend QuasiMonteCarloResult { QuasiMonteCarloResult(const QuasiMonteCarloResult & other) { return new OT::QuasiMonteCarloResult(other); } } }
