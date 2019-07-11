// SWIG file GeneralizedExtremeValueFactory.i

%{
#include "openturns/GeneralizedExtremeValueFactory.hxx"
%}

%include GeneralizedExtremeValueFactory_doc.i

%include openturns/GeneralizedExtremeValueFactory.hxx
namespace OT { %extend GeneralizedExtremeValueFactory { GeneralizedExtremeValueFactory(const GeneralizedExtremeValueFactory & other) { return new OT::GeneralizedExtremeValueFactory(other); } } }
