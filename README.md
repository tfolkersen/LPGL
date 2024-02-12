# LPGL (Lua Palette Graphics Library)
A graphics API inspired by PICO-8 and TIC-80, for making games in Lua and/or C++. Runs both natively and in a web browser

## Goals
#### Simplicity
LPGL is easy to use, and doesn't require the usual boilerplate code characteristic of writing OpenGL applications. Get started immediately by drawing shapes to the screen using simple functions!
#### Generality
LPGL is free of artificial limitations imposed by many fantasy consoles, such as fixed size palettes with fixed colors, fixed resolutions, or limits on the size of a user's application; no need to minify your code! Draw either with RGBA colors, or with palette indices, or a mix of both. The ability to draw to either stencil buffers or color buffers makes it possible to implement lighting, layers, and other visual effects. You can make a modern game, or a retro style game
#### Extendibility
The user can create new drawing functions which correctly interact with the rest of the API, with relative ease

## Roadmap

* Specification/design document
* Boilerplate + simple input demo
* Build system (ability to disable Lua; run in browser and natively)
* Vertical slice: drawing boxes, polygons, and sprites
* C++ graphics API
* Implement audio
* Complete Lua API (graphics + audio)
* (Maybe) make JS bindings
* (Maybe) make tools (sound editor, sprite editor, code editor, music editor)
* (Maybe in the future) add 3D drawing functions


## API Details

### Global state variables
##### target: (RGBA buffer) || (index buffer) || (stencil buffer)
The surface which all drawing functions modify

---

##### fillp: 64 bits
Bitmask of 8x8 tiled pattern affecting drawing functions

##### cliprect: x1, y1, x2, y2, flip (boolean default false)
Rectangle and boolean which prevent drawing outside the rectangle (or inside if flip is true)

##### stencil: (stencil buffer) || NULL
Prevent drawing outside of the region masked by the stencil

##### stencilflip: (boolean default false)
Invert on/off behaviour of bound stencil

##### stenciloffset: x, y
Coordinates of stencil

---

##### spritesheet: (RGBA buffer) || (index buffer) || NULL
Sprite sheet used as texture for drawing functions

##### spriteparams: width, height
Sprite tiling information

---

##### palette: (map from index -> RGBA) || NULL
Converts index buffer values to RGBA values

##### palettemap: (map from index -> index)
Mutates index buffer values (can be used to shift colors, i.e. fade-in/fade-out screen effect)

##### palettemask (map from index -> boolean)
Defines transparency for palette indices

---

##### camera: x, y, angle, zoom
Offsets drawing functions

##### screenshader: (screen shader effect)
Post processing shader used when drawing the internal framebuffer to the screen. Can be used for scanline effect

### Functions
Drawing functions taking color literals have two versions: one for indexed colors (i.e. rectfill) and one for RGBA colors (i.e. frectfill)

**drawopts** is [c1, c2, ang, sx, sy] or [r, g, b, a, ang, sx, sy]

##### [f]rectfill(x1, y1, x2, y2, drawopts)
##### [f]ovalfill(x, y, r1, r2, drawopts)
##### [f]circfill(x, y, r, drawopts)
TODO
