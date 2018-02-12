// SWIG file NaiveEnclosingSimplex.i

%{
#include "openturns/NaiveEnclosingSimplex.hxx"
%}

%include openturns/NaiveEnclosingSimplex.hxx
namespace OT { %extend NaiveEnclosingSimplex { NaiveEnclosingSimplex(const NaiveEnclosingSimplex & other) { return new OT::NaiveEnclosingSimplex(other); } } }
