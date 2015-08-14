// SWIG file TriangularFactory.i

%{
#include "TriangularFactory.hxx"
%}

%include TriangularFactory_doc.i

%include TriangularFactory.hxx
namespace OT { %extend TriangularFactory { TriangularFactory(const TriangularFactory & other) { return new OT::TriangularFactory(other); } } }
