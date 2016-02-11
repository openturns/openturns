// SWIG file InverseTrendTransform.i

%{
#include "openturns/InverseTrendTransform.hxx"
%}

%include InverseTrendTransform_doc.i

%include openturns/InverseTrendTransform.hxx
namespace OT { %extend InverseTrendTransform { InverseTrendTransform(const InverseTrendTransform & other) { return new OT::InverseTrendTransform(other); } } }
