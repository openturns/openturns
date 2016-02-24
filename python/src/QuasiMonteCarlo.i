// SWIG file QuasiMonteCarlo.i

%{
#include "openturns/QuasiMonteCarlo.hxx"
%}

%include QuasiMonteCarlo_doc.i

%include openturns/QuasiMonteCarlo.hxx
namespace OT{ %extend QuasiMonteCarlo { QuasiMonteCarlo(const QuasiMonteCarlo & other) { return new OT::QuasiMonteCarlo(other); } } }
