// SWIG file ChiSquareFactory.i

%{
#include "ChiSquareFactory.hxx"
%}

%include ChiSquareFactory_doc.i

%include ChiSquareFactory.hxx
namespace OT { %extend ChiSquareFactory { ChiSquareFactory(const ChiSquareFactory & other) { return new OT::ChiSquareFactory(other); } } }
