// SWIG file SecondOrderModel.i

%{
#include "openturns/SecondOrderModel.hxx"
%}

%include SecondOrderModel_doc.i

OTTypedInterfaceObjectHelper(SecondOrderModel)

%include openturns/SecondOrderModel.hxx
namespace OT{ %extend SecondOrderModel { SecondOrderModel(const SecondOrderModel & other) { return new OT::SecondOrderModel(other); } } }
