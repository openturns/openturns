%define OT_TemperatureProfile_doc
"Base class to define a temperature profile.

Notes
-----
The temperature profile is used in simulated annealing context.
"
%enddef
%feature("docstring") OT::TemperatureProfileImplementation
OT_TemperatureProfile_doc

// ---------------------------------------------------------------------

%define OT_TemperatureProfile_getIMax
"Maximum index value accessor.

Returns
-------
iMax : int
    The maximum index value."
%enddef
%feature("docstring") OT::TemperatureProfileImplementation::getIMax
OT_TemperatureProfile_getIMax

// ---------------------------------------------------------------------

%define OT_TemperatureProfile_getT0
"Temperature origin accessor.

Returns
-------
T0 : float
    The temperature origin."
%enddef
%feature("docstring") OT::TemperatureProfileImplementation::getT0
OT_TemperatureProfile_getT0
