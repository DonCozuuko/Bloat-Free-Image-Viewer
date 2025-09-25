# Bloat-Free-Image-Viewer ~bfiv 
A CLI tool that loads an image file into a bloat-free gui viewer.

I absolute hate windows' proprietary image viewer. It's filled to the brim with bloat; features that literally nobody uses, and as a result the app runs slower than fucking molasses going uphill. And so begun a tale as old as time, the fabled David and Goliath; little man versus the big man; the junior developer piercing into the souls of the Microsoft devs and (Microsoft middle management delegating them) that made this hideous piece of software, and challenging them by making my own blazingly fast, bloat-free, OPEN SOURCE image viewer; thus metaphorically, throwing down the gauntlet. 

I used c++ along with the raylib graphics library (openGL wrapper) to anyone curious. A note I feel is important: this image viewer is tailored to my preferences, so if you think it needs some other feature, feel free to make an issue if you care enough. 

## Features
- It loads images, duh-doy!
- Vim navigation for moving across the image when zoomed in.
- Displays necessary info such as:
	- Dimensions (pixels)
	- Size
	- File name + Full File Path

## Demo
![[assets/markichins.png]]


## Build from Source
```
$ make gui
```

## Usage
- I recommend making a bash alias to 
```
$ bfiv "filename.file"
$ bfiv filename.file
$ bfiv ./filename.file
$ bfiv ./dir1/dir2/filename.file
$ bfiv ../../filename.file
```