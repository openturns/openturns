// SWIG file MonteCarlo.i

%{
#include "openturns/MonteCarlo.hxx"
%}

%include MonteCarlo_doc.i

%include openturns/MonteCarlo.hxx
namespace OT{ %extend MonteCarlo { MonteCarlo(const MonteCarlo & other) { return new OT::MonteCarlo(other); } } }
