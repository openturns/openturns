// SWIG file RandomizedQuasiMonteCarlo.i

%{
#include "openturns/RandomizedQuasiMonteCarlo.hxx"
%}

%include RandomizedQuasiMonteCarlo_doc.i

%include openturns/RandomizedQuasiMonteCarlo.hxx
namespace OT { %extend RandomizedQuasiMonteCarlo { RandomizedQuasiMonteCarlo(const RandomizedQuasiMonteCarlo & other) { return new OT::RandomizedQuasiMonteCarlo(other); } } }
