# Some SFML Game Engine

## Requirements
* SFML
* LUA
* tinyxml2
* boost (for boost filesystem)

I plan on making a static build when I find out how to.

## Building
### Linux
```
$ cd build
$ make
```

### Mac/Apple
I have never built anything for Mac/Apple, sorry! I'm sure one of these days I'll figure it out.

### Windows
I don't have a working build for Windows, if I do it will most likely be seperate since I'm using a lot of Linux based libraries.


You CAN get it to compile using Visual Studio since I did that just today, but it's a pain in the butt and I still have unused includes that I have removed that won't compile on Visual Studio.


If you do want to go through the pain of getting this to compile on Windows in its current state you will need the following libs.
* boost_1_72_0 (build filesystem)
* lua (any version)
* SFML-2.5.1 (It will probably compile on newer versions though)
* tinyxml2 (the .cpp and .h file)
* sol/sol.h
From there if you set it up and remove the un-used/incompatible code it should work.

## Using the engine
In its current state it doesn't support modifications nor custom LUA. This will be updated when support is added.
You can edit the one map in resources/maps/untitled.tmx there is no check for custom files yet.
