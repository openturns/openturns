// SWIG file Gamma.i

%{
#include "openturns/Gamma.hxx"
%}

%include Gamma_doc.i

%include openturns/Gamma.hxx
namespace OT { %extend Gamma { Gamma(const Gamma & other) { return new OT::Gamma(other); } } }
