// SWIG file Histogram.i

%{
#include "Histogram.hxx"
%}

%include Histogram_doc.i

%include Histogram.hxx

%template(HistogramPairCollection) OT::Collection<OT::HistogramPair>;

%extend OT::Collection<OT::HistogramPair>
{

OT::Collection<OT::HistogramPair> (PyObject * pyObj)
{
  return OT::buildCollectionFromPySequence<OT::Histogram::Pair>( pyObj );
}

}


namespace OT {  

%extend Histogram {
  Histogram(NumericalScalar first, const OT::Collection<OT::HistogramPair> & collection)
  {
    return new OT::Histogram(first,collection);
  }
}

}

namespace OT { %extend Histogram { Histogram(const Histogram & other) { return new OT::Histogram(other); } } }
