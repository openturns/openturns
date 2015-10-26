// SWIG file SecondOrderModel.i

%{
#include "SecondOrderModel.hxx"
%}

%include SecondOrderModel_doc.i

OTTypedInterfaceObjectHelper(SecondOrderModel)

%include SecondOrderModel.hxx
namespace OT{ %extend SecondOrderModel { SecondOrderModel(const SecondOrderModel & other) { return new OT::SecondOrderModel(other); } } }
