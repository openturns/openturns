// SWIG file BarPlot.i

%{
#include "BarPlot.hxx"
%}

%include BarPlot_doc.i

%include BarPlot.hxx
namespace OT { %extend BarPlot { BarPlot(const BarPlot & other) { return new OT::BarPlot(other); } } }
