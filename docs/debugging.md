# Debugging

This guide explains how to debug the code. The provided configuration needs Visual Studio Code. For other IDEs, you need to adapt the configuration.
The whole configuration is provided in the `.vscode` folder. Everything should work out of the box. 

You have to create a docker container first. See [Getting Started](getting-started.md) for more information.

!!! Important
    After that you need to install the Visual Studio Code `Remote - Containers` and `C/C++` extensions. You can find them [here](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) and [here](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) respectively. 

After that, you will find a docker icon in left bar. If you click it, it will show you the list of containers. You should see the container you created before. If you right click on it and select `Attach Visual Studio Code`, it will open a new window with the container attached.

!!! important
    You have to install the `C/C++`  extension in the container too. You find the link [here](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools).

Then you can open the folder `/messy` and click on the debug icon (green circle in the picture below) in the left bar. You should see a list of debug configurations. The one we are interested in is called `gdb` (red circle in the picture below). If you click on it, it will start the debugger. You can set breakpoints and step through the code.

![debugging](images/debugging.png)