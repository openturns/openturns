// SWIG file LinearModelAlgorithm.i

%{
#include "openturns/LinearModelAlgorithm.hxx"
%}

%include LinearModelAlgorithm_doc.i

%rename(LinearModelAlgorithm_operator___eq__) OT::operator ==(const LinearModelAlgorithm & lhs, const LinearModelAlgorithm & rhs);

%include openturns/LinearModelAlgorithm.hxx
namespace OT { %extend LinearModelAlgorithm { LinearModelAlgorithm(const LinearModelAlgorithm & other) { return new OT::LinearModelAlgorithm(other); } } }
