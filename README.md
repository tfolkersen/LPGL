# LPGL (Lua Palette Graphics Library)
A graphics API inspired by PICO-8 and TIC-80, for making games in Lua and C++. Runs both natively and in a web browser

## Motivation

## Roadmap


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
