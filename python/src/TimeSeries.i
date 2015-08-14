// SWIG file TimeSeries.i

%{
#include "TimeSeries.hxx"
%}

%include TimeSeries_doc.i

%include TimeSeries.hxx
namespace OT { %extend TimeSeries { TimeSeries(const TimeSeries & other) { return new OT::TimeSeries(other); } } }
