// SWIG file GeometricFactory.i

%{
#include "GeometricFactory.hxx"
%}

%include GeometricFactory_doc.i

%include GeometricFactory.hxx
namespace OT { %extend GeometricFactory { GeometricFactory(const GeometricFactory & other) { return new OT::GeometricFactory(other); } } }
