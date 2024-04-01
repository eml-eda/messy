Hello World
===========

Requirements
------------

No specific requirement. This example should run without issue on all chips/boards/OSes.

Description
-----------

This example is a classic Hello World.
It prints an hello world string on all available cores.

How to run
----------

.. tabs::

   .. tab:: Makefile

      .. tabs::

         .. tab:: Real board

            .. code-block:: bash

               make clean all run platform=board

         .. tab:: GVSoC

            .. code-block:: bash

               make clean all run platform=gvsoc

   .. tab:: CMake

      .. tabs::

         .. tab:: Real board

            .. code-block:: bash

                  cmake -B build
                  cmake --build build --target menuconfig # Select your board in the menu
                  cmake --build build --target run

         .. tab:: GVSoC

            .. code-block:: bash

                  cmake -B build
                  cmake --build build --target menuconfig # Select GVSoC in the menu
                  cmake --build build --target run

Results
-------

You should have an output looking like this (order may vary):

.. code-block::

         *** PMSIS HelloWorld ***

    Entering main controller
    [32 0] Hello World!
    Cluster master core entry
    [0 2] Hello World!
    [0 0] Hello World!
    [0 1] Hello World!
    [0 3] Hello World!
    [0 4] Hello World!
    [0 5] Hello World!
    [0 6] Hello World!
    [0 7] Hello World!
    Cluster master core exit
    Bye !

Code
----

.. tabs::

   .. tab:: C

      .. literalinclude:: helloworld.c
         :language: c

   .. tab:: Makefile

      .. literalinclude:: Makefile
         :language: Makefile

   .. tab:: CMakelist

         .. literalinclude:: CMakeLists.txt
            :language: CMake

   .. tab:: KConfig

         .. literalinclude:: Kconfig
            :language: KConfig
