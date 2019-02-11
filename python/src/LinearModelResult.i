// SWIG file LinearModelResult.i

%{
#include "openturns/LinearModelResult.hxx"
%}

%include LinearModelResult_doc.i

//%rename(LinearModelResult_operator___eq__) OT::operator ==(const LinearModelResult & lhs, const LinearModelResult & rhs);

%include openturns/LinearModelResult.hxx
namespace OT { %extend LinearModelResult { LinearModelResult(const LinearModelResult & other) { return new OT::LinearModelResult(other); } } }
