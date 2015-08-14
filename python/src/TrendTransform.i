// SWIG file TrendTransform.i

%{
#include "TrendTransform.hxx"
%}

%include TrendTransform_doc.i

%include TrendTransform.hxx
namespace OT { %extend TrendTransform { TrendTransform(const TrendTransform & other) { return new OT::TrendTransform(other); } } }
