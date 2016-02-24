// SWIG file CenteredFiniteDifferenceGradient.i

%{
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
%}

%include CenteredFiniteDifferenceGradient_doc.i

%include openturns/CenteredFiniteDifferenceGradient.hxx
namespace OT {  
%extend CenteredFiniteDifferenceGradient {

 CenteredFiniteDifferenceGradient(const CenteredFiniteDifferenceGradient & other)
 {
   return new OT::CenteredFiniteDifferenceGradient(other);
 }

 CenteredFiniteDifferenceGradient(const NumericalPoint & epsilon,
                                  const NumericalMathEvaluationImplementation & evaluation)
 {
   return new OT::CenteredFiniteDifferenceGradient(epsilon, evaluation.clone());
 }

 CenteredFiniteDifferenceGradient(const NumericalScalar epsilon,
                                  const NumericalMathEvaluationImplementation & evaluation)
 {
   return new OT::CenteredFiniteDifferenceGradient(epsilon, evaluation.clone());
 }


}

}
