// SWIG file Dirac.i

%{
#include "Dirac.hxx"
%}

%include Dirac_doc.i

%include Dirac.hxx
namespace OT { %extend Dirac { Dirac(const Dirac & other) { return new OT::Dirac(other); } } }
