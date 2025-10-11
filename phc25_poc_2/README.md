# PHC-25 Mode 12 Tetris POC

This is an improved proof-of-concept for Tetris on the Thomson PHC-25 using Mode 12 (256x192 monochrome graphics).

## Key Improvements Over POC 1

- **Higher Resolution**: 256x192 pixels instead of 128x192
- **Proper Proportions**: 8x8 pixel blocks look much better
- **Official Layout**: 10x20 Tetris playfield (standard size)
- **Crisp Graphics**: Monochrome but much sharper appearance
- **Optimized Drawing**: Each block = 8 bytes (1 byte per row)

## Screen Layout

```
Total: 256x192 pixels
┌─────────────────────────────────────────────────────────┐
│ Title Area (8 pixels high)                             │
├──────────┬─────────────────────┬─────────────────────────┤
│ Left UI  │ Playfield (80x160)  │ Right UI               │
│ (72px)   │ 10x20 blocks        │ (72px)                 │
│          │ 8x8 pixels each     │                        │
│          │                     │                        │
│          │ Centered at (88,16) │                        │
└──────────┴─────────────────────┴─────────────────────────┘
```

## Technical Details

- **Graphics Mode**: Mode 12 (256x192, 1 bit per pixel)
- **Block Size**: 8x8 pixels (perfect for Tetris)
- **Memory Usage**: 6KB VRAM, each row = 32 bytes
- **Block Drawing**: Optimized - 8 memory writes per block
- **Colors**: CSS=0 gives black/green (authentic retro feel)

## Demo Features

- All 7 standard Tetris pieces (I, T, L, O, S, Z, J)
- Proper 10x20 playfield with borders
- Simulated filled lines at bottom
- UI areas for score/level (placeholder graphics)
- Decorative borders and layout

## Building

```bash
make          # Build the demo
make clean    # Clean build artifacts
make info     # Show build configuration
```

## Expected Result

The demo should display a proper Tetris layout with:
- Sharp, correctly-proportioned 8x8 blocks
- All Tetris pieces in their classic shapes
- Professional-looking bordered playfield
- Space for UI elements on the sides

This should look much better than the chunky, stretched blocks from POC 1!