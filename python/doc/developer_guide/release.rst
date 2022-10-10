.. _release:

===============
Release process
===============

1. New "release" branch

2. Reindent sources (utils/reindent.sh)

3. Increment version number X.Yrc1 (utils/setVersionNumber.sh)

4. Increment rpm package number (distro/rpm)

5. Update Debian package (distro/debian):

  - Sync with `debian package <https://packages.debian.org/source/sid/openturns>`_
  - Check for new patches
  - New entry in changelog
  - Update soversion in rules, rename install files

6. Build new packages on `OBS <https://build.opensuse.org/project/monitor/science:openturns>`_

7. Upload debian packages using `scripts <https://github.com/openturns/build-deb/>`_

8. Merge release branch and tag new version

9. Build `Windows binaries <https://github.com/openturns/build/>`_

10. Build `Windows modules binaries <https://github.com/openturns/build-modules>`_

11. Update conda packages `recipe <https://github.com/conda-forge/openturns-feedstock>`_

12. Update conda modules packages `recipes <https://github.com/conda-forge?q=ot>`_

13. Update `otconda <https://github.com/openturns/otconda>`_ bundle

14. Update `wheels <https://github.com/openturns/build-wheels>`_

15. Set version X.Y+1, new changelog in master

16. Drop deprecated methods listed in TODO file

17. Bump soversion (lib/src/CMakeLists.txt)
