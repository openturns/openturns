// SWIG file LeastSquaresFactory.i

%{
#include "openturns/LeastSquaresFactory.hxx"
%}

%include LeastSquaresFactory_doc.i

%ignore OT::LeastSquaresFactory::buildParameter;

%include openturns/LeastSquaresFactory.hxx
namespace OT { %extend LeastSquaresFactory { LeastSquaresFactory(const LeastSquaresFactory & other) { return new OT::LeastSquaresFactory(other); } } }
