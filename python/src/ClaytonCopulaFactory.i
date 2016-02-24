// SWIG file ClaytonCopulaFactory.i

%{
#include "openturns/ClaytonCopulaFactory.hxx"
%}

%include ClaytonCopulaFactory_doc.i

%include openturns/ClaytonCopulaFactory.hxx
namespace OT { %extend ClaytonCopulaFactory { ClaytonCopulaFactory(const ClaytonCopulaFactory & other) { return new OT::ClaytonCopulaFactory(other); } } }
