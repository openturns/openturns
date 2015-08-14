// SWIG file DirichletFactory.i

%{
#include "DirichletFactory.hxx"
%}

%include DirichletFactory_doc.i

%include DirichletFactory.hxx
namespace OT { %extend DirichletFactory { DirichletFactory(const DirichletFactory & other) { return new OT::DirichletFactory(other); } } }
