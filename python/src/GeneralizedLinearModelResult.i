// SWIG file GeneralizedLinearModelResult.i

%{
#include "openturns/GeneralizedLinearModelResult.hxx"
%}

%include GeneralizedLinearModelResult_doc.i

%include openturns/GeneralizedLinearModelResult.hxx

namespace OT{ %extend GeneralizedLinearModelResult { GeneralizedLinearModelResult(const GeneralizedLinearModelResult & other) { return new OT::GeneralizedLinearModelResult(other); } } }
