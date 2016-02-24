// SWIG file TriangularFactory.i

%{
#include "openturns/TriangularFactory.hxx"
%}

%include TriangularFactory_doc.i

%include openturns/TriangularFactory.hxx
namespace OT { %extend TriangularFactory { TriangularFactory(const TriangularFactory & other) { return new OT::TriangularFactory(other); } } }
