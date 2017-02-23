// SWIG file GeneralLinearModelAlgorithm.i

%{
#include "openturns/GeneralLinearModelAlgorithm.hxx"
%}

%include GeneralLinearModelAlgorithm_doc.i

%include openturns/GeneralLinearModelAlgorithm.hxx

namespace OT{ %extend GeneralLinearModelAlgorithm { GeneralLinearModelAlgorithm(const GeneralLinearModelAlgorithm & other) { return new OT::GeneralLinearModelAlgorithm(other); } } }

%pythoncode %{
# deprecated
class GeneralizedLinearModelAlgorithm(GeneralLinearModelAlgorithm):
    def __init__(self, *args):
        super(GeneralizedLinearModelAlgorithm, self).__init__(*args)
        openturns.common.Log.Warn('class GeneralizedLinearModelAlgorithm is deprecated in favor of GeneralLinearModelAlgorithm')
%}
