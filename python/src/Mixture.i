// SWIG file Mixture.i

%{
#include "Mixture.hxx"
%}

%include Mixture_doc.i

%include Mixture.hxx
namespace OT { %extend Mixture { Mixture(const Mixture & other) { return new OT::Mixture(other); } } }
