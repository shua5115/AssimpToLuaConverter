# AssimpToLuaConverter
A program that dumps aiData into lua source code. How you use the data is up to you.
This is intended to be used with love2d.

The purpose of this program is to be able to load various 3D file formats (with animations) into love2d for 3D rendering.
This exists because I didn't want to modify the love2d executable to have this functionality because c++ is infuriating.
I spent so long trying to debug why the program crashed without errors, only to realize I was dereferencing a null pointer.

However, the tradeoff for decreased (initial) coding time is inflated file sizes (binary formats like FBX have an over 10x increase),
slower file loading (many conversion steps), and increased system complexity (many conversion steps).

For those curious, for my use case, these are the conversion steps:

Binary file from 3D software -> assimp data structures -> lua source code (this library).
Then, after the lua source code is generated,
load source code from disk -> 
reformat data to be shader-friendly, load vertex data into love2d meshes, and add nodes to game world

Because of the aforementioned tradeoffs, I have resolved that I shouldn't spend more time on this solution.
Look out for a fork of love2d with various add-ons, because I am practically forced to do so to achieve the goal of a 3D engine in love2d.
