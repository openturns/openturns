// SWIG file RandomizedQuasiMonteCarlo.i

%{
#include "RandomizedQuasiMonteCarlo.hxx"
%}

%include RandomizedQuasiMonteCarlo_doc.i

%include RandomizedQuasiMonteCarlo.hxx
namespace OT { %extend RandomizedQuasiMonteCarlo { RandomizedQuasiMonteCarlo(const RandomizedQuasiMonteCarlo & other) { return new OT::RandomizedQuasiMonteCarlo(other); } } }
