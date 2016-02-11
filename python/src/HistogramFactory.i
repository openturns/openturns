// SWIG file HistogramFactory.i

%{
#include "openturns/HistogramFactory.hxx"
%}

%include HistogramFactory_doc.i

%include openturns/HistogramFactory.hxx
namespace OT { %extend HistogramFactory { HistogramFactory(const HistogramFactory & other) { return new OT::HistogramFactory(other); } } }
