// SWIG file Mixture.i

%{
#include "openturns/Mixture.hxx"
%}

%include Mixture_doc.i

%include openturns/Mixture.hxx
namespace OT { %extend Mixture { Mixture(const Mixture & other) { return new OT::Mixture(other); } } }
