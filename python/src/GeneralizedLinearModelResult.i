// SWIG file GeneralizedLinearModelResult.i

%{
#include "GeneralizedLinearModelResult.hxx"
%}

%include GeneralizedLinearModelResult_doc.i

%include GeneralizedLinearModelResult.hxx

namespace OT{ %extend GeneralizedLinearModelResult { GeneralizedLinearModelResult(const GeneralizedLinearModelResult & other) { return new OT::GeneralizedLinearModelResult(other); } } }
