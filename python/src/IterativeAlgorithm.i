// SWIG file IterativeAlgorithm.i

%{
#include "openturns/IterativeAlgorithm.hxx"
%}

%include IterativeAlgorithm_doc.i

%include openturns/IterativeAlgorithm.hxx
namespace OT {
%extend IterativeAlgorithm {

IterativeAlgorithm(const IterativeAlgorithm & other)
{
return new OT::IterativeAlgorithm(other);
}

} // IterativeAlgorithm
} // OT

