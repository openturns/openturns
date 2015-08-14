// SWIG file QuasiMonteCarlo.i

%{
#include "QuasiMonteCarlo.hxx"
%}

%include QuasiMonteCarlo_doc.i

%include QuasiMonteCarlo.hxx
namespace OT{ %extend QuasiMonteCarlo { QuasiMonteCarlo(const QuasiMonteCarlo & other) { return new OT::QuasiMonteCarlo(other); } } }
