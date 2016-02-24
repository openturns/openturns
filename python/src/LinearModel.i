// SWIG file LinearModel.i

%{
#include "openturns/LinearModel.hxx"
%}

%include LinearModel_doc.i

%rename(LinearModel_operator___eq__) OT::operator ==(const LinearModel & lhs, const LinearModel & rhs);

%include openturns/LinearModel.hxx
namespace OT{ %extend LinearModel { LinearModel(const LinearModel & other) { return new OT::LinearModel(other); } } }
