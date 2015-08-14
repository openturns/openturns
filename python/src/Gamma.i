// SWIG file Gamma.i

%{
#include "Gamma.hxx"
%}

%include Gamma_doc.i

%include Gamma.hxx
namespace OT { %extend Gamma { Gamma(const Gamma & other) { return new OT::Gamma(other); } } }
