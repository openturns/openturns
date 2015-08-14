// SWIG file ARMACoefficients.i

%{
#include "ARMAState.hxx"
%}

%include ARMAState_doc.i


%include ARMAState.hxx


namespace OT { %extend ARMAState { ARMAState(const ARMAState & other) { return new OT::ARMAState(other); } } }


