// SWIG file OrderStatisticsMarginalChecker.i

%{
#include "OrderStatisticsMarginalChecker.hxx"
%}

%include OrderStatisticsMarginalChecker_doc.i

%include OrderStatisticsMarginalChecker.hxx
namespace OT { %extend OrderStatisticsMarginalChecker { OrderStatisticsMarginalChecker(const OrderStatisticsMarginalChecker & other) { return new OT::OrderStatisticsMarginalChecker(other); } } }
