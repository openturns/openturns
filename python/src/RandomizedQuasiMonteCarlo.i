// SWIG file RandomizedQuasiMonteCarlo.i

%{
#include "RandomizedQuasiMonteCarlo.hxx"
%}

%include RandomizedQuasiMonteCarlo.hxx
namespace OT { %extend RandomizedQuasiMonteCarlo { RandomizedQuasiMonteCarlo(const RandomizedQuasiMonteCarlo & other) { return new OT::RandomizedQuasiMonteCarlo(other); } } }
