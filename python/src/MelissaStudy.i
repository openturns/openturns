// SWIG file MelissaStudy.i

%{
#include "openturns/MelissaStudy.hxx"
%}

%include openturns/MelissaStudy.hxx
namespace OT{
%extend MelissaStudy { 
MelissaStudy(const MelissaStudy & other) 
{
return new OT::MelissaStudy(other);
}

}
}
