// SWIG file QuadraticBasisFactory.i

%{
#include "QuadraticBasisFactory.hxx"
%}

%include QuadraticBasisFactory_doc.i 

%include QuadraticBasisFactory.hxx
namespace OT { %extend QuadraticBasisFactory { QuadraticBasisFactory(const QuadraticBasisFactory & other) { return new OT::QuadraticBasisFactory(other); } } }
