// SWIG file GeneralLinearModelResult.i

%{
#include "openturns/GeneralLinearModelResult.hxx"
%}

%include GeneralLinearModelResult_doc.i

%include openturns/GeneralLinearModelResult.hxx

namespace OT{ %extend GeneralLinearModelResult { GeneralLinearModelResult(const GeneralLinearModelResult & other) { return new OT::GeneralLinearModelResult(other); } } }

%pythoncode %{
# deprecated
class GeneralizedLinearModelResult(GeneralLinearModelResult):
    def __init__(self, *args):
        super(GeneralizedLinearModelResult, self).__init__(*args)
        openturns.common.Log.Warn('class GeneralizedLinearModelResult is deprecated in favor of GeneralLinearModelResult')
%}


