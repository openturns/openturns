// SWIG file TrendTransform.i

%{
#include "openturns/TrendTransform.hxx"
%}

%include TrendTransform_doc.i

%include openturns/TrendTransform.hxx
namespace OT { %extend TrendTransform { TrendTransform(const TrendTransform & other) { return new OT::TrendTransform(other); } } }
