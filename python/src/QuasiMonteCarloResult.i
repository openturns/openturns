// SWIG file QuasiMonteCarloResult.i

%{
#include "openturns/QuasiMonteCarloResult.hxx"
%}

%include QuasiMonteCarloResult_doc.i

%include openturns/QuasiMonteCarloResult.hxx
namespace OT{ %extend QuasiMonteCarloResult { QuasiMonteCarloResult(const QuasiMonteCarloResult & other) { return new OT::QuasiMonteCarloResult(other); } } }
