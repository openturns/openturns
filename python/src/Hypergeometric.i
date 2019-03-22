// SWIG file Hypergeometric.i

%{
#include "openturns/Hypergeometric.hxx"
%}

%include Hypergeometric_doc.i

%include openturns/Hypergeometric.hxx
namespace OT { %extend Hypergeometric { Hypergeometric(const Hypergeometric & other) { return new OT::Hypergeometric(other); } } }
