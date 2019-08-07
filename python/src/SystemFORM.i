// SWIG file SystemFORM.i

%{
#include "openturns/SystemFORM.hxx"
%}

%include SystemFORM_doc.i

%include openturns/SystemFORM.hxx
namespace OT { %extend SystemFORM { SystemFORM(const SystemFORM & other) { return new OT::SystemFORM(other); } } }
