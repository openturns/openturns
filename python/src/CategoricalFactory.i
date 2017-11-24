// SWIG file CategoricalFactory.i

%{
#include "openturns/CategoricalFactory.hxx"
%}

%include CategoricalFactory_doc.i

%include openturns/CategoricalFactory.hxx
namespace OT { %extend CategoricalFactory { CategoricalFactory(const CategoricalFactory & other) { return new OT::CategoricalFactory(other); } } }

%pythoncode %{
class UserDefinedFactory(CategoricalFactory):
    def __init__(self, *args):
        super(UserDefinedFactory, self).__init__(*args)
        openturns.common.Log.Warn('class UserDefinedFactory is deprecated in favor of CategoricalFactory')
%}

