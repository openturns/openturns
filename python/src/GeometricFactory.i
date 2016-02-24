// SWIG file GeometricFactory.i

%{
#include "openturns/GeometricFactory.hxx"
%}

%include GeometricFactory_doc.i

%include openturns/GeometricFactory.hxx
namespace OT { %extend GeometricFactory { GeometricFactory(const GeometricFactory & other) { return new OT::GeometricFactory(other); } } }
