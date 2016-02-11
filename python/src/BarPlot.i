// SWIG file BarPlot.i

%{
#include "openturns/BarPlot.hxx"
%}

%include BarPlot_doc.i

%include openturns/BarPlot.hxx
namespace OT { %extend BarPlot { BarPlot(const BarPlot & other) { return new OT::BarPlot(other); } } }
