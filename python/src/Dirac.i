// SWIG file Dirac.i

%{
#include "openturns/Dirac.hxx"
%}

%include Dirac_doc.i

%include openturns/Dirac.hxx
namespace OT { %extend Dirac { Dirac(const Dirac & other) { return new OT::Dirac(other); } } }
