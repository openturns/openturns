// SWIG file LegendreFactory.i

%{
#include "LegendreFactory.hxx"
%}

%include LegendreFactory_doc.i

%include LegendreFactory.hxx
namespace OT { %extend LegendreFactory { LegendreFactory(const LegendreFactory & other) { return new OT::LegendreFactory(other); } } }
