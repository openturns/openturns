=============
Configuration
=============

.. currentmodule:: openturns

Environment variables
=====================

OpenTURNS uses the following (optional) environment variables.

OPENTURNS_CONFIG_PATH
    Path to the `openturns.conf` XML file used by the :class:`~openturns.ResourceMap`.

OPENTURNS_HOME
    Path to the OpenTURNS installation directory.

OPENTURNS_LOG_SEVERITY
    The default severity of logs, see the :class:`~openturns.Log` class.

Environment checking and filesystem manipulation
================================================

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

Threading configuration
=======================

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    TBB_Enable
    TBB_Disable
