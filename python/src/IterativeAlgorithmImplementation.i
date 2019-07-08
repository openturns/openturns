// SWIG file IterativeAlgorithm.i

%{
#include "openturns/IterativeAlgorithmImplementation.hxx"
%}

%include IterativeAlgorithmImplementation_doc.i

%include openturns/IterativeAlgorithmImplementation.hxx
namespace OT {
%extend IterativeAlgorithm {

IterativeAlgorithmImplementation(const IterativeAlgorithmImplementation & other)
{
return new OT::IterativeAlgorithmImplementation(other);
}

} // IterativeAlgorithmImplementation
} // OT

