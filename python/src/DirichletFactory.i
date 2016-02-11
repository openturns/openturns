// SWIG file DirichletFactory.i

%{
#include "openturns/DirichletFactory.hxx"
%}

%include DirichletFactory_doc.i

%include openturns/DirichletFactory.hxx
namespace OT { %extend DirichletFactory { DirichletFactory(const DirichletFactory & other) { return new OT::DirichletFactory(other); } } }
