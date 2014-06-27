# graphics engine

[![Build Status][travis_image]][travis_status]

A proof-of-concept graphics engine, implemented in `C`.

### compilation/installation
 * `make all` : compile the engine.
 * `make test` : run unit tests
 * `make clean` : delete compiled binaries
 * `make install` : install all of the engine's dependencies, which are:
	* [SDL][SDL]
	* [ncurses][ncurses]
	* [Xlib][Xlib]

### features
This project implements a rudimentary graphics engine from the ground up, using
the `SDL` graphics library only for per-pixel access. The following algorithms
and features are included:

 * Bresenham line rasterization
 * translations, scaling, rotations
 * Bezier/Hermite curves
 * shape creation: spheres, tori, rectangular prisms
 * backface culling
 * scanline fills
 * z-buffering
 * a custom scripting language, `MDL`, with a `Lex`/`Yacc`parser and
    interpreter
 * Goraud shading
 * a unit-testing suite

### use
##### basics
From the project root, run `make install`, and `make all`. When the engine finishes compilation, run `make test` to
execute the engine's full suite of unit-tests: make sure they all succeeded! If one doesn't (and you're checked out to
`master`), you should try another computer. Then, run `make run` to see a sample animation.

##### more
Beyond the compiled animation displayed by `make run`, check out the following:

 * **animations**: the engine comes pre-packaged with animation `gif`s inside `animations/`.
 * **scripting**: the engine has its own scripting language, `MDL`, which allows you to toy with its numerous features.
   Read [scripting](#scripting) for more details.

### scripting
The engine's scripts are written in a language called `MDL`, which consists of commands written on individual lines. A
simple script might look like:

	// A sample script that creates a sphere, then rotates, scales, and translates it.

	push
	rotate x 45
	move 10 30 40
	scale 2 2 2
	sphere 0 0 0 50
	display

###### stack/other commands
 * `pop`
 * `push`
 * `display`
 * `save`
 * `//`

###### transformations
 * `move`
 * `rotate`
 * `scale`

##### image creation
 * `box`
 * `torus`
 * `sphere`
 * `line`

[SDL]: http://www.libsdl.org/
[ncurses]: http://www.gnu.org/software/ncurses/
[Xlib]:http://en.wikipedia.org/wiki/Xlib
[travis_status]: (https://travis-ci.org/sevko/graphics-engine)
[travis_image]: https://travis-ci.org/sevko/large-pixel-collider.svg
