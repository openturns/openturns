// SWIG file LinearModelAlgorithm.i

%{
#include "openturns/LinearModelAlgorithm.hxx"
%}

%include LinearModelAlgorithm_doc.i

%rename(LinearModelAlgorithm_operator___eq__) OT::operator ==(const LinearModelAlgorithm & lhs, const LinearModelAlgorithm & rhs);

%copyctor OT::LinearModelAlgorithm;

%include openturns/LinearModelAlgorithm.hxx
