// SWIG file QuadraticBasisFactory.i

%{
#include "openturns/QuadraticBasisFactory.hxx"
%}

%include QuadraticBasisFactory_doc.i 

%include openturns/QuadraticBasisFactory.hxx
namespace OT { %extend QuadraticBasisFactory { QuadraticBasisFactory(const QuadraticBasisFactory & other) { return new OT::QuadraticBasisFactory(other); } } }
