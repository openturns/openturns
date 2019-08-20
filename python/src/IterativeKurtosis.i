// SWIG file IterativeKurtosis.i

%{
#include "openturns/IterativeKurtosis.hxx"
%}

%include IterativeKurtosis_doc.i

%include openturns/IterativeKurtosis.hxx

namespace OT {
%extend IterativeKurtosis {

IterativeKurtosis(const IterativeKurtosis & other)
{
return new OT::IterativeKurtosis(other);
}

} // IterativeKurtosis
} // OT

