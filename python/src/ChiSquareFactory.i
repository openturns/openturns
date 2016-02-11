// SWIG file ChiSquareFactory.i

%{
#include "openturns/ChiSquareFactory.hxx"
%}

%include ChiSquareFactory_doc.i

%include openturns/ChiSquareFactory.hxx
namespace OT { %extend ChiSquareFactory { ChiSquareFactory(const ChiSquareFactory & other) { return new OT::ChiSquareFactory(other); } } }
