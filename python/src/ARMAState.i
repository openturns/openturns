// SWIG file ARMACoefficients.i

%{
#include "openturns/ARMAState.hxx"
%}

%include ARMAState_doc.i


%include openturns/ARMAState.hxx


namespace OT { %extend ARMAState { ARMAState(const ARMAState & other) { return new OT::ARMAState(other); } } }


