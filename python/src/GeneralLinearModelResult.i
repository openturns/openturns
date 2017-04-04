// SWIG file GeneralLinearModelResult.i

%{
#include "openturns/GeneralLinearModelResult.hxx"
%}

%include GeneralLinearModelResult_doc.i

%include openturns/GeneralLinearModelResult.hxx

namespace OT{ %extend GeneralLinearModelResult { GeneralLinearModelResult(const GeneralLinearModelResult & other) { return new OT::GeneralLinearModelResult(other); } } }


