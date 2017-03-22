// SWIG file GeneralizedExtremeValue.i

%{
#include "openturns/GeneralizedExtremeValue.hxx"
%}

%include GeneralizedExtremeValue_doc.i

%include openturns/GeneralizedExtremeValue.hxx
namespace OT { %extend GeneralizedExtremeValue { GeneralizedExtremeValue(const GeneralizedExtremeValue & other) { return new OT::GeneralizedExtremeValue(other); } } }
