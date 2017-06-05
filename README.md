# What is CrashOS?

CrashOS is a tool dedicated to the research of vulnerabilities in hypervisors by creating unusual system configurations. 
CrashOS is a minimalist Operating System which aims to lead to hypervisor crashs, hence its name. 
You can launch existing tests or implement your owns and observe hypervisor behaviour towards this unusual kernel.

The core of CrashOS provides the following OS features: 
- the Boot entry;
- the memory management (segmentation and paging);
- the interrupt and exception handling;
- the I/O communication.

A default kernel configuration is available but this set of features allows to entirely reconfigure the kernel as you desire.

# Hardware and software requirements
CrashOS only works on Intel x86 hardware architecture, requires gcc-4.8 to be compiled and GRUB to boot.

# Installation
To install CrashOS, first compile the project with the main Makefile. It will create the 32-bits executable [test.bin](https://github.com/airbus-seclab/crashos/blob/master/build/test.bin).
`.../crashos$ make`

Then install test.bin and Grub in a bootable storage, and use this bootable storage to launch the VM in your hypervisor.
A example of installation with Vmware is included in the Makefile by executing the following command line:

`.../crashos$ make install`

Don't forget to adapt the VM path in the script [tools/installer_vmware.sh](https://github.com/airbus-seclab/crashos/blob/master/tools/installer_vmware.sh): `VMPATH="/home/xxx/Vmware/${VMNAME}"`

# Usage

Use the script [create_new_test_directory.py](https://github.com/airbus-seclab/crashos/blob/master/tools/create_new_test_directory.py) to create a new directory containing your test. 
It will create the local Makefile, a log file to store the test logs, a text file to describe the test and the test file filled with the test template.

    /crashos/tools$ python create_new_test_directory.py myowntest
    Directory myowntest created
    /crashos/tools$ cd ..
    /crashos$ ls src/myowntest/
    Makefile  myowntest.c  myowntest.log  myowntest.txt

In CrashOS, each test needs to define a specific kernel configuration. Thus, each test must contain:

- an "init" function: it saves the current kernel configuration and defines the configuration with which we want to work;
- the "test" function;
- a "restore" function: it recovers the old kernel configuration.

A test template is available [here](https://github.com/airbus-seclab/crashos/blob/master/templates/test_template.txt).

To init the kernel, some default functions are available ([init.h](https://github.com/airbus-seclab/crashos/blob/master/src/core/init.h) and [init.c](https://github.com/airbus-seclab/crashos/blob/master/src/core/init.c)):

|       Function        |                         Description                             |                                                           
|-----------------------|-----------------------------------------------------------------|
|init_work_mem() 		    | Initialize the mem_info struct to define the available physical memory |
|init_segmentation(...)	| Initialize the GDT (Global Descriptor Table) with the following entries and update gdtr and segment selectors |
|init_paging(...) 		  | Initialize the PGD with the first 4MB in Identity Mapping, update CR3 and CR4 and enable the paging in CR0    |
|init_interrupts(...) 	| Initialize the IDT (Interrupt Descriptor Table) with the following entries (32 first entries for exceptions)  |     

Others functions allow the developer to modify the default system parameters and to define his own configuration. The following command line generates a a code documentation for all functions available in CrashOS:
    `.../crashos$ make doc`
It will create a html documentation in doxygen_documentation/html/index.html.

Specify, in the main [Makefile](https://github.com/airbus-seclab/crashos/blob/master/Makefile), the list of tests you want to launch:
    
    ...
    TESTS :=  test_repo1 test_repo2 
    ...

Then, compile the project.
    `.../crashos$ make install`

# License
CrashOS is released under [GPLv2](https://github.com/airbus-seclab/crashos/blob/master/LICENSE.md).
