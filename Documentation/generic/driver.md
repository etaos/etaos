# Writing a driver
[TOC]

\section driver-intro Introduction

So, you have a chip on your hands that isn't supported by ETA/OS yet. How about
you bring that support to us and write a driver for it.

This document will focus on the 'how to write a driver' part. Information on
how this driver should be submitted to ETA/OS can be found in the generic
contribution guide.

\section modules Drivers & modules

Almost all drivers in ETA/OS are setup as modules. This means that they can
be compiled, but are not neccessarily used by an application. This gives
an application more flexibility when it comes to recompiling ETA/OS. Moreover,
modules are automatically initialised so the user of a driver (or module) doesn't
need to bother about calling complicated init functions.

\subsection prep Preparation

You should prepare yourself before you begin writing actual code. Make sure
you have all related documents at hand such as:

* The datasheet for your MCU
* The datasheet for the chip you are writing a driver for
* This website for the ETA/OS API documentation

Please note that we, at ETA/OS, like well designed code. So please think about
that aswell: how are you going to design your driver? What interface is it
going to get? Are you going to use a function based interface, like the GPIO
driver, or is it better to use the VFS as interface (such as the EEPROM and
SRAM drivers)?

\subsection setup Setting up the driver module

Every module in ETA/OS contains an init call, such as `module_init`. There
are several of these and the only difference is order of execution. All
available init calls, in order of execution are:

* `subsys_init`
* `module_init`
* `device_init`
* `chip_init`

Once you've decided which init call your driver needs you can setup the module:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
void yourdrivername_init(void)
{
	/* Do nice init stuff here */
}

module_init(yourdrivername_init);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

And.. that is it. That is all ETA/OS needs to know about your driver to properly
initialise it.A

\subsection final Final words
Now you are ready to start writing code for your driver. For more information
on the driver API you can take a look at the [device driver core](@ref dev) and
the [virtual file system](@ref vfs).

