// SWIG file TrendFactory.i

%{
#include "TrendFactory.hxx"
%}


%include TrendFactory_doc.i

%include TrendFactory.hxx
namespace OT { %extend TrendFactory { TrendFactory(const TrendFactory & other) { return new OT::TrendFactory(other); } } }
