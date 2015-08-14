// SWIG file LinearTaylor.i

%{
#include "LinearTaylor.hxx"
%}

%include LinearTaylor_doc.i

%include LinearTaylor.hxx
namespace OT { %extend LinearTaylor { LinearTaylor(const LinearTaylor & other) { return new OT::LinearTaylor(other); } } }
