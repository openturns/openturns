Git workflow
============

Proposing a pull request
------------------------

Patches are welcome to our library repository: https://github.com/openturns/openturns.

Here are the steps required to propose a modification with git:

1. Setup git for first time use::

    git config --global user.name "John Doe"
    git config --global user.email "johndoe@example.com"
    git config --global branch.autosetuprebase remote


2. Register on `GitHub <https://github.com>`_ and add a new ssh key in https://github.com/settings/ssh::

    ssh-keygen -f ~/.ssh/id_rsa-github
    cat ~/.ssh/id_rsa-github.pub


3. Fork the library repository https://github.com/openturns/openturns and clone it via ssh::

    git clone git@github.com:doe/openturns.git doe
    cd doe


4. Create a branch from master with a relevant topic name::

    git checkout -b branch_name master


5. Commit your changes, split your work in as much commits as needed to ease reviewing::

    git add <files> # Add current state of files to commit
    git commit
    ...
    git add <files>
    git commit

To write a nice commit message, keep a short commit title (less than 50 characters),
leave a blank line, then add a mor detailed description.

6. Push the new branch to your personal repository::

    git push --set-upstream origin branch_name


7. Create a pull request on https://github.com/openturns/openturns

That's it, your code is ready for reviewing, but it may not be accepted as is.

Checkout the comments made after your pull request and the automated test result.

You may want to push more commits to that branch as required.

Propose a separate pull request for each topic you want to address.

Here are more resources on `using pull requests <https://help.github.com/articles/using-pull-requests/>`_
and `Git <https://git-scm.com/book/en/v2>`_.


Keep your local repository in sync
----------------------------------

You may want to keep your personal repository in sync with the main repository
before starting new developments.

1. Add upstream remote repository::

    git remote add upstream https://github.com/openturns/openturns.git


2. Retrieve usptream::

    git fetch upstream


3. Merge upstream master branch to your local personal master branch::

    git checkout master
    git merge upstream/master


4. Push to your remote personal repository::

    git push


Delete a branch
---------------

Once your pull-request is merged, you may want to delete the branch.

1. Delete local branch::

    git branch -d branch_name


2. Delete remote branch::

    git push origin :branch_name


Tagging a release
-----------------

0. List existing tags::

    git tag


1. Get the master branch::

    git checkout master


2. Create the tag on local repository::

    git tag -a v2.0 -m 'version 2.0'


3. Push the tag on the remote repository::

    git push origin v2.0
