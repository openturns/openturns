// SWIG file Categorical.i



%{
#include "openturns/Categorical.hxx"
%}

%include Categorical_doc.i

%include openturns/Categorical.hxx


namespace OT {  

%extend Categorical {

Categorical(const Categorical & other)
{
  return new OT::Categorical(other);
} 

} // Categorical

} // OT

%pythoncode %{
class UserDefined(Categorical):
    def __init__(self, *args):
        super(UserDefined, self).__init__(*args)
        openturns.common.Log.Warn('class UserDefined is deprecated in favor of Categorical')
%}

