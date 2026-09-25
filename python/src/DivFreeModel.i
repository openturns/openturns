// SWIG file DivFreeModel.i

%{
#include "openturns/DivFreeModel.hxx"
%}

%ignore OT::DivFreeModel::operator=(const DivFreeModel &);

%include DivFreeModel_doc.i

%copyctor OT::DivFreeModel;

%include openturns/DivFreeModel.hxx
