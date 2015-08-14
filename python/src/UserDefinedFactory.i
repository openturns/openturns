// SWIG file UserDefinedFactory.i

%{
#include "UserDefinedFactory.hxx"
%}

%include UserDefinedFactory.hxx
namespace OT { %extend UserDefinedFactory { UserDefinedFactory(const UserDefinedFactory & other) { return new OT::UserDefinedFactory(other); } } }
