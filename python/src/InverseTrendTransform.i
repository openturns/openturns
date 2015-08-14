// SWIG file InverseTrendTransform.i

%{
#include "InverseTrendTransform.hxx"
%}

%include InverseTrendTransform_doc.i

%include InverseTrendTransform.hxx
namespace OT { %extend InverseTrendTransform { InverseTrendTransform(const InverseTrendTransform & other) { return new OT::InverseTrendTransform(other); } } }
