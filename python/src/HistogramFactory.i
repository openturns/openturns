// SWIG file HistogramFactory.i

%{
#include "HistogramFactory.hxx"
%}

%include HistogramFactory_doc.i

%include HistogramFactory.hxx
namespace OT { %extend HistogramFactory { HistogramFactory(const HistogramFactory & other) { return new OT::HistogramFactory(other); } } }
