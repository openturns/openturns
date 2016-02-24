// SWIG file LinearTaylor.i

%{
#include "openturns/LinearTaylor.hxx"
%}

%include LinearTaylor_doc.i

%include openturns/LinearTaylor.hxx
namespace OT { %extend LinearTaylor { LinearTaylor(const LinearTaylor & other) { return new OT::LinearTaylor(other); } } }
