// SWIG file CenteredFiniteDifferenceGradient.i

%{
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
%}

%include CenteredFiniteDifferenceGradient_doc.i

%copyctor OT::CenteredFiniteDifferenceGradient;
%include openturns/CenteredFiniteDifferenceGradient.hxx
namespace OT {  
%extend CenteredFiniteDifferenceGradient {

 CenteredFiniteDifferenceGradient(const Point & epsilon,
                                  const EvaluationImplementation & evaluation)
 {
   return new OT::CenteredFiniteDifferenceGradient(epsilon, evaluation.clone());
 }

 CenteredFiniteDifferenceGradient(const Scalar epsilon,
                                  const EvaluationImplementation & evaluation)
 {
   return new OT::CenteredFiniteDifferenceGradient(epsilon, evaluation.clone());
 }

}

}
