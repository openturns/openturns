// SWIG file UserDefinedFactory.i

OTAutodoc(UserDefinedFactory)

%{
#include "UserDefinedFactory.hxx"
%}

%include UserDefinedFactory.hxx
namespace OT { %extend UserDefinedFactory { UserDefinedFactory(const UserDefinedFactory & other) { return new OT::UserDefinedFactory(other); } } }
