// SWIG file UserDefinedFactory.i

OTAutodoc(UserDefinedFactory)

%{
#include "openturns/UserDefinedFactory.hxx"
%}

%include openturns/UserDefinedFactory.hxx
namespace OT { %extend UserDefinedFactory { UserDefinedFactory(const UserDefinedFactory & other) { return new OT::UserDefinedFactory(other); } } }
