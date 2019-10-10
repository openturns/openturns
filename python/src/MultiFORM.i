// SWIG file MultiFORM.i

%{
#include "openturns/MultiFORM.hxx"
%}

%include MultiFORM_doc.i

%include openturns/MultiFORM.hxx
namespace OT { %extend MultiFORM { MultiFORM(const MultiFORM & other) { return new OT::MultiFORM(other); } } }
