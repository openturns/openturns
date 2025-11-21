%feature("docstring") OT::PlatformInfo
"Informations about platform.

No constructor, only static methods are available."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetDate
"Accessor to the build date.

Returns
-------
date : str
    Build date."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetInstallationDirectory
"Accessor to the install prefix.

Returns
-------
prefix : str
    Installation prefix."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetVersion
"Accessor to the version string.

Returns
-------
version : str
    Version string."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetRevision
"Accessor to the revision string.

Returns
-------
revision : str
    Git revision."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::SetNumericalPrecision
"Accessor to the output numerical precision.

Parameters
----------
precision : int
    String output numerical precision."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetNumericalPrecision
"Accessor to the output numerical precision.

Returns
-------
precision : int
    String output numerical precision."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetFeatures
"Accessor to the features list.

Returns
-------
features : :class:`~openturns.Description`
    Features list.

Examples
--------
>>> import openturns as ot
>>> print(ot.PlatformInfo.GetFeatures())
[bonmin,...
"

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::HasFeature
"Test a feature's availability.

Parameters
----------
feature : str
    The desired feature, often associated to a software dependency,
    Call :meth:`GetFeatures` for the possible feature values.

Returns
-------
has_feature : bool
    Whether the designated feature is available.

Examples
--------
>>> import openturns as ot
>>> have_xml_support = ot.PlatformInfo.HasFeature('libxml2')
"

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetCompilerId
"Accessor to the compiler id.

Returns
-------
id : str
    Compiler id (GNU, MSVC...)."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetCompilerVersion
"Accessor to the compiler version.

Returns
-------
version : str
    Compiler version."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetSystemName
"Accessor to the system name.

Returns
-------
name : str
    System name."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetSystemVersion
"Accessor to the system version.

Returns
-------
version : str
    System version."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::GetSystemProcessor
"Accessor to the system processor.

Returns
-------
processor : str
    System processor."

// ---------------------------------------------------------------------

%feature("docstring") OT::PlatformInfo::Summary
"Accessor to infos summary.

Returns
-------
summary : str
    Summary of version, compiler, features..."
