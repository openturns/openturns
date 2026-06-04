%feature("docstring") OT::StorageManager
"Define the way OpenTURNS' objects are saved and reloaded.

Available constructors:
    StorageManager(*defaultVersion=1*)

Parameters
----------
defaultVersion : positive int
    Argument interpreted as the name of the object in a user point of view.
    This name is never used in another way than for user information. This name
    has no meaning to the platform.

Notes
-----
A StorageManager object can be used only through its derived classes
including :class:`~openturns.XMLStorageManager` and its HDF5 counterpart.

See also
--------
XMLStorageManager"

// ---------------------------------------------------------------------

%feature("docstring") OT::StorageManager::getDefaultStudyVersion
"Return the version of the study that the manager can read/write.

Returns
-------
version : positive int
    Version of the study that the manager can read/write."

// ---------------------------------------------------------------------

%feature("docstring") OT::StorageManager::getStudyVersion
"Get the study version.

Returns
-------
version : positive int
    Study version."

// ---------------------------------------------------------------------

%feature("docstring") OT::StorageManager::setStudyVersion
"Set the study version.

Parameters
----------
version : positive int
    Study version."
