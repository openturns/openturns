// SWIG file CurlFreeModel.i

%{
#include "openturns/CurlFreeModel.hxx"
%}

%ignore OT::CurlFreeModel::operator=(const CurlFreeModel &);

%include CurlFreeModel_doc.i

%copyctor OT::CurlFreeModel;

%include openturns/CurlFreeModel.hxx
