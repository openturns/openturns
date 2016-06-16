// SWIG file LARS.i

%{
#include "openturns/LARS.hxx"
%}

%include LARS_doc.i

%include openturns/LARS.hxx
namespace OT { %extend LARS { LARS(const LARS & other) { return new OT::LARS(other); } } }

%pythoncode %{
# deprecated
class LAR(LARS):
    def __init__(self, *args):
        super(LAR, self).__init__(*args)
        openturns.common.Log.Warn('class LAR is deprecated in favor of LARS')
%}
