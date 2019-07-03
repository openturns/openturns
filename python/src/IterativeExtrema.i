// SWIG file IterativeExtrema.i

%{
#include "openturns/IterativeExtrema.hxx"
%}

%include IterativeExtrema_doc.i

%include openturns/IterativeExtrema.hxx

namespace OT {
%extend IterativeExtrema {

IterativeExtrema(const IterativeExtrema & other)
{
return new OT::IterativeExtrema(other);
}

} // IterativeExtrema
} // OT

