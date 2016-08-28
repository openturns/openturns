// SWIG file HistogramPolynomialFactory.i

%{
#include "openturns/HistogramPolynomialFactory.hxx"
%}

%include HistogramPolynomialFactory_doc.i

%include openturns/HistogramPolynomialFactory.hxx
namespace OT { %extend HistogramPolynomialFactory { HistogramPolynomialFactory(const HistogramPolynomialFactory & other) { return new OT::HistogramPolynomialFactory(other); } } }
