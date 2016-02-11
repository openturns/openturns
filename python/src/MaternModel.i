// SWIG file MaternModel.i

%{
#include "openturns/MaternModel.hxx"
%}

%include MaternModel_doc.i

%include openturns/MaternModel.hxx
namespace OT { %extend MaternModel { MaternModel(const MaternModel & other) { return new OT::MaternModel(other); } } }
