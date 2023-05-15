// SWIG file GeneralizedExtremeValueValidation.i

%{
#include "openturns/GeneralizedExtremeValueValidation.hxx"
%}

%include GeneralizedExtremeValueValidation_doc.i

%include openturns/GeneralizedExtremeValueValidation.hxx
namespace OT { %extend GeneralizedExtremeValueValidation { GeneralizedExtremeValueValidation(const GeneralizedExtremeValueValidation & other) { return new OT::GeneralizedExtremeValueValidation(other); } } }
