// SWIG file Ipopt.i

%{
#include "openturns/Ipopt.hxx"
%}

%include Ipopt_doc.i

%include openturns/Ipopt.hxx

namespace OT { %extend Ipopt { Ipopt(const Ipopt & other) { return new OT::Ipopt(other); } } }
