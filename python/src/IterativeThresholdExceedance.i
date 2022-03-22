// SWIG file IterativeThresholdExceedance.i

%{
#include "openturns/IterativeThresholdExceedance.hxx"
%}

%include IterativeThresholdExceedance_doc.i

%include openturns/IterativeThresholdExceedance.hxx

namespace OT {
%extend IterativeThresholdExceedance {

IterativeThresholdExceedance(const IterativeThresholdExceedance & other)
{
return new OT::IterativeThresholdExceedance(other);
}

} // IterativeThresholdExceedance
} // OT

