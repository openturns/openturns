// SWIG file GeneralizedPareto.i

%{
#include "openturns/GeneralizedPareto.hxx"
%}

%include GeneralizedPareto_doc.i

%include openturns/GeneralizedPareto.hxx
namespace OT { %extend GeneralizedPareto { GeneralizedPareto(const GeneralizedPareto & other) { return new OT::GeneralizedPareto(other); } } }
