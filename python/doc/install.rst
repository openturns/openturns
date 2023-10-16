.. _install:

============
Installation
============

From binary packages
====================

Pip
---
Install binary packages from `PyPI <https://pypi.org/project/openturns/>`_::

    pip3 install openturns

For an installation without administrative privileges the option :code:`--user` can be used.
Note that pip does not install pre-releases unless given the option :code:`--pre`.

Conda
-----
This is relevant to the `Conda <http://conda.pydata.org/>`_ userland Python package manager.

As binary dependency packages from the `conda-forge <https://conda-forge.org>`_
channel are not compatible with ones from the default channel, openturns packages
are not working on top of Anaconda.
Instead, we recommend installing conda from `Miniforge <https://github.com/conda-forge/miniforge>`_
where conda is configured to prioritize packages from conda-forge out of the box.
This can also be achieved with Miniconda with extra steps.
On Windows just download the matching miniforge exe and follow the instructions,
on Linux you can install it from command-line in one go::

    wget https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh -P /tmp
    bash /tmp/Miniforge3-Linux-x86_64.sh -b -p $HOME/miniforge
    PATH="$HOME/miniforge/bin:$PATH"

Then it should be ready to install packages::

    conda install -y openturns

Note that conda can be slow or fail at resolving complex environments with many packages
so when a full upgrade is needed our advice is to create a new environment from scratch
(see also `Mamba <https://github.com/mamba-org/mamba/>`_).

Alternatively, you can download the `otconda <https://github.com/openturns/otconda>`_ bundle
containing the library and its modules that allows for an offline installation.
Note that pre-releases are published on conda-forge like regular releases, so
if you want to stick to the stable version you will have to explicitly specify
the required version, eg :code:`conda install openturns=1.20`.

Windows
-------
Download the installer matching your python version and architecture from `GitHub <https://github.com/openturns/build/releases>`_
Then just run installer and follow the instructions.

A non-interactive installation is also possible with the command line::

    openturns-1.20-py38-x86_64.exe /userlevel=1 /S /FORCE /D=%PREFIX%

macOS
-----
Installation options include pip and conda.

Debian/Ubuntu
-------------

First create an entry in the apt configuration matching your distribution family and codename
if available in one of the following commands::

    echo deb https://openturns.github.io/apt/ubuntu jammy main | sudo tee /etc/apt/sources.list.d/openturns.list
    echo deb https://openturns.github.io/apt/ubuntu focal main | sudo tee /etc/apt/sources.list.d/openturns.list
    echo deb https://openturns.github.io/apt/debian bookworm main | sudo tee /etc/apt/sources.list.d/openturns.list
    echo deb https://openturns.github.io/apt/debian bullseye main | sudo tee /etc/apt/sources.list.d/openturns.list

.. note::

    Use the bash command `lsb_release -a` in order to determine the codename of your distribution.

Then add the mirror signature, enable apt https support and update the package database::

    curl -s https://openturns.github.io/apt/public.key | sudo apt-key add -
    sudo apt install apt-transport-https
    sudo apt update

At this point the following new packages should be available via the package manager:

.. table::

    +----------------------+------------------------------------+
    | Package name         | Description                        |
    +======================+====================================+
    | libopenturns0.x      | library                            |
    +----------------------+------------------------------------+
    | libopenturns-dev     | development package                |
    +----------------------+------------------------------------+
    | python3-openturns    | Python module                      |
    +----------------------+------------------------------------+

To install the Python module::

    sudo apt install python3-openturns

RPM-based distributions
-----------------------
Add the repository corresponding to your operating system::

    curl https://download.opensuse.org/repositories/science:/openturns/CentOS_8/science:openturns.repo -o /etc/yum.repos.d/science-openturns.repo
    curl https://download.opensuse.org/repositories/science:/openturns/Fedora_38/science:openturns.repo -o /etc/yum.repos.d/science-openturns.repo
    curl https://download.opensuse.org/repositories/science:/openturns/Fedora_37/science:openturns.repo -o /etc/yum.repos.d/science-openturns.repo
    curl https://download.opensuse.org/repositories/science:/openturns/15.4/science:openturns.repo -o /etc/zypp/repos.d/science-openturns.repo
    curl https://download.opensuse.org/repositories/science:/openturns/Mageia_8/science:openturns.repo -o /etc/yum.repos.d/science-openturns.repo

Import the gpg key corresponding to your operating system::

    rpm --import https://download.opensuse.org/repositories/science:/openturns/CentOS_8/repodata/repomd.xml.key
    rpm --import https://download.opensuse.org/repositories/science:/openturns/Fedora_38/repodata/repomd.xml.key
    rpm --import https://download.opensuse.org/repositories/science:/openturns/Fedora_37/repodata/repomd.xml.key
    rpm --import https://download.opensuse.org/repositories/science:/openturns/15.4/repodata/repomd.xml.key
    rpm --import https://download.opensuse.org/repositories/science:/openturns/Mageia_8/repodata/repomd.xml.key

The following packages are available:

.. table::

    +----------------------+------------------------------------+
    | Package name         | Description                        |
    +======================+====================================+
    | openturns-libs       | library                            |
    +----------------------+------------------------------------+
    | openturns-devel      | development package                |
    +----------------------+------------------------------------+
    | python3-openturns    | Python module                      |
    +----------------------+------------------------------------+

To install the Python module::

    yum install python3-openturns

Archlinux
---------
Install from `AUR <https://aur.archlinux.org/packages/openturns/>`_::

    aurman -S openturns

FreeBSD
-------
Install from `FreshPorts <https://www.freshports.org/math/openturns/>`_::

    pkg install openturns

Development version
===================

From source
-----------
Install the required :ref:`dependencies` and build the development branch::

    git clone https://github.com/openturns/openturns.git
    cd openturns
    cmake -DCMAKE_INSTALL_PREFIX=~/.local .
    make install

.. note::

    When installing into `~/.local` you don't need to set PYTHONPATH environment variable for Python to be able to import openturns

Weekly builds
-------------
Weekly built Python binaries are also available with the latest bugfixes and features, but also with new bugs and API changes, use at your own risk::

    pip3 install --pre --extra-index-url https://pypi.anaconda.org/openturns-wheels-nightly/simple --upgrade --force-reinstall openturns
