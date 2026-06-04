.. _release:

===============
Release process
===============

1. New "release" branch

2. Reindent sources (utils/indent.sh)

3. Drop dev version suffix (utils/setVersionNumber.sh X.Y)

4. Increment rpm package number (distro/rpm)

5. Update Debian package (distro/debian):

  - Sync with `debian package <https://packages.debian.org/source/sid/openturns>`_
  - Check for new patches
  - New entry in changelog
  - Update soversion in control, rename install files

6. Build new packages on `OBS <https://build.opensuse.org/project/monitor/science:openturns>`_

7. Upload debian packages using `scripts <https://github.com/openturns/build-deb/>`_

8. Merge release branch, create X.Y branch, and tag new version

9. Update conda packages `recipe <https://github.com/conda-forge/openturns-feedstock>`_

10. Update conda modules packages `recipes <https://github.com/conda-forge?q=ot>`_

11. Update `otconda <https://github.com/openturns/otconda>`_ bundle

12. Update `wheels <https://github.com/openturns/build-wheels>`_

13. Update `doc <https://github.com/openturns/openturns.github.io>`_ latest link (copy folder X.Y into latest).

14. Add discourse announcement topic.

15. Update `website <https://github.com/openturns/www>`_, with latest doc link.

16. Set version X.Y+1dev, new changelog in master

17. Bump soversion (lib/src/CMakeLists.txt)

18. Drop deprecated methods listed in TODO file

