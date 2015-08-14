// SWIG file MaternModel.i

%{
#include "MaternModel.hxx"
%}

%include MaternModel_doc.i

%include MaternModel.hxx
namespace OT { %extend MaternModel { MaternModel(const MaternModel & other) { return new OT::MaternModel(other); } } }
