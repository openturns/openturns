// SWIG file OrderStatisticsMarginalChecker.i

%{
#include "openturns/OrderStatisticsMarginalChecker.hxx"
%}

%include OrderStatisticsMarginalChecker_doc.i

%include openturns/OrderStatisticsMarginalChecker.hxx
namespace OT { %extend OrderStatisticsMarginalChecker { OrderStatisticsMarginalChecker(const OrderStatisticsMarginalChecker & other) { return new OT::OrderStatisticsMarginalChecker(other); } } }
