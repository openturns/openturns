// SWIG file ClaytonCopulaFactory.i

%{
#include "ClaytonCopulaFactory.hxx"
%}

%include ClaytonCopulaFactory_doc.i

%include ClaytonCopulaFactory.hxx
namespace OT { %extend ClaytonCopulaFactory { ClaytonCopulaFactory(const ClaytonCopulaFactory & other) { return new OT::ClaytonCopulaFactory(other); } } }
