# Contributing to ETA/OS

First off, thanks for taking the time to contribute to ETA/OS. Below you'll find
a set of guidelines for contributing to ETA/OS (and any of its side 
projects/scripts). The first thing to note is that these are project guidelines,
not strict rules, if you feel like something needs to change within this document;
propose it on the mailing list or sent in a pull request with your changes.

#### Table of contents
[Before you start](#before-you-start)
* [Code of Conduct](#code-of-conduct)

[How to contribute](#how-to-contribute)
  * [Reporting bugs](#reporting-bugs)
  * [Suggestions](#suggestions)
  * [Code contributions](#code-contributions)
  * [Patches and pull requests](#patches-and-pull-requests)

[Style guide](#style-guide)
  * [Git](#git-commit-messages)
  * [C/C++](#cc)
  * [Documentation](#documentation)

## Before you start

### Code of conduct
ETA/OS adheres to the Contributor Covenant code of conduct. By contributing, you are
expected to uphold this code.

## How to contribute

### Reporting bugs
This section will guide you through to steps to create a proper bug report. These
steps are created to make it easier for the mainainers and developers to understand
your bug report, reproduce the wrongful behaviour and find possibly related bug
reports.

#### Filing a bug report
Bugs in the ETA/OS project are tracked using the GitLab issue tracker. Once 
you've opened up a new issue, follow the checklist below to create the bug report.

  * **Use a clear and more importantly, descriptive title** for the issue to identify
    the problem.
  * **Describe the hardware you were using**. This includes every piece. Every chip,
    every resistor. For the more complicated setups, including an electrical design is
    highly reccomended.
  * **Provide us your ETA/OS configuration file**. Include a link to the .config file
    you used to compile ETA/OS.
  * **Provide the exact steps which reproduce the problem**. Do not shy away from
    excessive detail here; there is no such thing. Look at it this way; if no one is
    able to understand how you've found this bug, no one is going to be able to fix
    it. **Don't just say what you did, include how you did it**.
  * **Include the version of ETA/OS you were using**.

You can provide more context by:

  * **Including code gists**. We'd love to hear what you thing triggers the bug.
  * **Including a list of things you already tried**. Have you check for stack
    overflows for example?

### Suggestions

There is two types of suggestions you can make: a well defined new feature, and an
idea. The fundamental difference between the two is how well the suggestion is
developed. The former is, as the name implies, a well rounded suggestion - ready to be
implemented. These kind of suggestions are tracked as GitLab issue's. See the
checklist below to see what information such a suggestion has to contain. Because
we don't want to kill any idea's you can always put less well rounded suggestions on
the [mailing list](mailto:etaos@googlegroups.com). Please note that public API
functions should not be changed. Their internals can be changed, but not the
return value or its parameters.

#### How to submit a proper suggestion

  * **Use a clear and descriptive title** for the issue
  * **Provide a step by step description of the suggested feature**. Use as much
    detail as you can here.
  * **If its not a new feature**, but a suggestion to update an already existing
    feature, **describe its current behaviour**, and the **behaviour you expected
    to see** (and why).
  * **Provide code snippets and software design diagrams (UML)**, to further explain
    your suggestion.

### Code contributions

Not sure where to start? Start in the issue tracker, bugs and features under the
[Documentation](https://git.bietje.net/etaos/etaos/issues?label_name=Documentation)
and [Device drivers](https://git.bietje.net/etaos/etaos/issues?label_name=Device+drivers)
labels are generally a good starting point. One should be aware that the
[Kernel](https://git.bietje.net/etaos/etaos/issues?label_name=Kernel)
and [Scheduling](https://git.bietje.net/etaos/etaos/issues?label_name=Scheduling)
labels generally contain more advanced features/bugs.

### Pull requests

Below you'll find the pull request checklist. Please follow this guideline as
close as possible when submitting a new patch.

  * Make sure your [code](#cc) and [documentation](#documentation) follows 
    the style guides.
  * Write your commit messages following the 'git commit' [checklist](#git-commit-messages).
  * Supply your code with test(s).
  * Describe which hardware you have used to test your patch.

## Style guide

Please try and follow the style guides below at all times. Keep in mind that patches
**can be rejected** based on style errors.

### Git commit messages
  * Use the present tense ("Add feature", not "Added feature").
  * On the first line, include subsystem (and module) your patch belongs to (e.g.
    "sched: core: optimized __schedule_need_resched()").
  * Limit the first line to 72 characters or less
  * If applicable, include references to issue's and pull requests.

### C/C++

ETA/OS follows a slightly modified version of the Linux kernel C/C++ style guide.
There is a separate [document](http://bietje.net/etaos-site/docs/codestyle.html)
available containing the entire C/C++ style guide.

### Documentation

Public API functions are documented using Doxygen. An example:

```C
/**
 * @brief Read from the LM35 temperature sensor.
 * @param pin Analog pin the sensor is connected to.
 * @return The temperature.
 *
 * This function returns the temperature using the ADC. The returned value is the
 * temperature in Celcius.
 */
int lm35_read(int pin)
```

