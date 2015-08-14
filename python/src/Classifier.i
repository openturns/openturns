// SWIG file Classifier.i

%{
#include "Classifier.hxx"
%}

%include Classifier_doc.i

OTTypedInterfaceObjectHelper(Classifier)

%include Classifier.hxx
namespace OT { %extend Classifier { Classifier(const Classifier & other) { return new OT::Classifier(other); } } }

