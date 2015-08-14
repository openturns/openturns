=============
Configuration
=============

.. currentmodule:: openturns

Environment variables
=====================

OpenTURNS uses the following (optional) environment variables.

OPENTURNS_CONFIG_PATH
    Path to the `openturns.conf` XML file used by the :class:`~openturns.ResourceMap`.

OPENTURNS_WRAPPER_PATH
    Path to compiled wrappers.

OPENTURNS_HOME
    Path to the OpenTURNS installation directory.

OPENTURNS_LOG_SEVERITY
    The default severity of logs, see the :class:`~openturns.Log` class.

OpenTURNS environment checking and filesystem manipulation
==========================================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Path

Information about the library
=============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    PlatformInfo

Recording user information
==========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Log
    TTY

Resource catalog
================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ResourceMap