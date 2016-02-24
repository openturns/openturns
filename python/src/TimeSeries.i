// SWIG file TimeSeries.i

%{
#include "openturns/TimeSeries.hxx"
%}

%include TimeSeries_doc.i

%include openturns/TimeSeries.hxx
namespace OT { %extend TimeSeries { TimeSeries(const TimeSeries & other) { return new OT::TimeSeries(other); } } }
