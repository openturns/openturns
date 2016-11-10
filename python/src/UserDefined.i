// SWIG file UserDefined.i



%{
#include "openturns/UserDefined.hxx"
%}

%include UserDefined_doc.i

%include openturns/UserDefined.hxx


namespace OT {  

%extend UserDefined {

UserDefined(const UserDefined & other)
{
  return new OT::UserDefined(other);
} 

} // UserDefined

} // OT

