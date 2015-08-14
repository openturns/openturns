// SWIG file MonteCarlo.i

%{
#include "MonteCarlo.hxx"
%}

%include MonteCarlo_doc.i

%include MonteCarlo.hxx
namespace OT{ %extend MonteCarlo { MonteCarlo(const MonteCarlo & other) { return new OT::MonteCarlo(other); } } }
