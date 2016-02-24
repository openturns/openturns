// SWIG file GeneralizedExponential.i

%{
#include "openturns/GeneralizedExponential.hxx"
%}

%include GeneralizedExponential_doc.i

%include openturns/GeneralizedExponential.hxx
namespace OT { %extend GeneralizedExponential { GeneralizedExponential(const GeneralizedExponential & other) { return new OT::GeneralizedExponential(other); } } }
