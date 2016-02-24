// SWIG file FourierSeriesFactory.i

%{
#include "openturns/FourierSeriesFactory.hxx"
%}

%include FourierSeriesFactory_doc.i

%include openturns/FourierSeriesFactory.hxx
namespace OT{ %extend FourierSeriesFactory { FourierSeriesFactory(const FourierSeriesFactory & other) { return new OT::FourierSeriesFactory(other); } } }
