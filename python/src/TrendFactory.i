// SWIG file TrendFactory.i

%{
#include "openturns/TrendFactory.hxx"
%}


%include TrendFactory_doc.i

%include openturns/TrendFactory.hxx
namespace OT { %extend TrendFactory { TrendFactory(const TrendFactory & other) { return new OT::TrendFactory(other); } } }
