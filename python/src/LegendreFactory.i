// SWIG file LegendreFactory.i

%{
#include "openturns/LegendreFactory.hxx"
%}

%include LegendreFactory_doc.i

%include openturns/LegendreFactory.hxx
namespace OT { %extend LegendreFactory { LegendreFactory(const LegendreFactory & other) { return new OT::LegendreFactory(other); } } }
