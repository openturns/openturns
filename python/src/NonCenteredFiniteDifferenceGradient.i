// SWIG file NonCenteredFiniteDifferenceGradient.i

%{
#include "openturns/NonCenteredFiniteDifferenceGradient.hxx"
%}

%include NonCenteredFiniteDifferenceGradient_doc.i

%copyctor OT::NonCenteredFiniteDifferenceGradient;
%include openturns/NonCenteredFiniteDifferenceGradient.hxx
namespace OT {  
%extend NonCenteredFiniteDifferenceGradient {

 NonCenteredFiniteDifferenceGradient(const Point & epsilon,
                                     const EvaluationImplementation & evaluation)
 {
   return new OT::NonCenteredFiniteDifferenceGradient(epsilon, evaluation.clone());
 }

 NonCenteredFiniteDifferenceGradient(const Scalar epsilon,
                                     const EvaluationImplementation & evaluation)
 {
   return new OT::NonCenteredFiniteDifferenceGradient(epsilon, evaluation.clone());
 }

}

}
