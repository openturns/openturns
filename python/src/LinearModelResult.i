// SWIG file LinearModelResult.i

%{
#include "openturns/LinearModelResult.hxx"
%}

%include LinearModelResult_doc.i

//%rename(LinearModelResult_operator___eq__) OT::operator ==(const LinearModelResult & lhs, const LinearModelResult & rhs);

%copyctor OT::LinearModelResult;

%include openturns/LinearModelResult.hxx
