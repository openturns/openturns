// SWIG file ThreeDVAR.i

%{
#include "openturns/ThreeDVAR.hxx"
%}

%include ThreeDVAR_doc.i

%include openturns/ThreeDVAR.hxx
namespace OT { %extend ThreeDVAR { ThreeDVAR(const ThreeDVAR & other) { return new OT::ThreeDVAR(other); } } }
