# GitHub Actions & Workflows

The `build_qmake.yml` in this directory adds a [GitHub action][1] and workflow that builds
your project anytime you push commits to GitHub on Windows, Linux and macOS.

The build artifacts can be downloaded from GitHub and be installed integrated in your projects

When you push a tag, the workflow also creates a new release on GitHub.

## Keeping it up to date

Near the top of the file you find a section starting with `env:`.

The value for `QT_VERSION` specifies the Qt version to use for building the plugin.

## What it does

The build job consists of several steps:

* Install required packages on the build host
* Download, unpack and install the binary for the Qt version
* Build the project and upload the binaries to GitHub
* If a tag is pushed, create a release on GitHub for the tag, including zipped plugin libraries
  for download

[1]: https://help.github.com/en/actions/automating-your-workflow-with-github-actions/about-github-actions
