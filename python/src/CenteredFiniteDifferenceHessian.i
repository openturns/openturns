// SWIG file CenteredFiniteDifferenceHessian.i

%{
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
%}

%include CenteredFiniteDifferenceHessian_doc.i

%include openturns/CenteredFiniteDifferenceHessian.hxx
namespace OT {  
%extend CenteredFiniteDifferenceHessian {

 CenteredFiniteDifferenceHessian(const CenteredFiniteDifferenceHessian & other)
 {
   return new OT::CenteredFiniteDifferenceHessian(other);
 }

 CenteredFiniteDifferenceHessian(const Point & epsilon,
                                 const EvaluationImplementation & evaluation)
 {
   return new OT::CenteredFiniteDifferenceHessian(epsilon, evaluation.clone());
 }

 CenteredFiniteDifferenceHessian(const NumericalScalar epsilon,
                                 const EvaluationImplementation & evaluation)
 {
   return new OT::CenteredFiniteDifferenceHessian(epsilon, evaluation.clone());
 }

}

}
