# PHC25 Tetris Implementation Report

## Overview

This report documents the proof-of-concept development for implementing Tetris on the Sanyo PHC-25 microcomputer. Two major approaches were tested to determine the optimal graphics strategy for the full implementation.

## POC 1: Mode 11 (128x192, 4-color) - FAILED

### Technical Specifications
- **Graphics Mode**: Mode 11 (128x192 pixels, 4-color)
- **Block Size**: Initially 10x8, then optimized to 8x8 pixels
- **Color Palette**: Green/Yellow/Blue/Red (CSS=0)
- **Memory Usage**: 6KB VRAM, ~800 bytes code

### Implementation Details
- Used 2 bits per pixel encoding (4 pixels per byte)
- Implemented optimized pixel plotting with lookup tables
- Character-aligned 8x8 blocks for 4-pixel boundary alignment
- Fast block drawing with full-byte operations

### Critical Problems Identified
1. **Poor Visual Quality**: Blocks appeared too large and horizontally stretched
2. **Low Resolution**: Only 15x24 block grid possible, insufficient for proper Tetris
3. **Aspect Ratio Issues**: MC6847 pixel aspect ratio caused distorted appearance
4. **Chunky Graphics**: Blocks looked unprofessional and hard to distinguish

### Conclusion: Rejected
Mode 11 proved unsuitable for Tetris due to fundamental resolution and aspect ratio limitations.

## POC 2: Mode 12 (256x192, Monochrome) - SUCCESS

### Technical Specifications
- **Graphics Mode**: Mode 12 (256x192 pixels, monochrome)
- **Block Size**: 8x8 pixels (optimal proportions)
- **Resolution**: 32x24 block grid (allows standard 10x20 Tetris playfield)
- **Colors**: Black background, green/white foreground
- **Memory Usage**: 6KB VRAM, ~1600 bytes code

### Implementation Highlights

#### Graphics Initialization
```c
// Mode 12: GM1=1, GM0=1, A/G=1
mode_byte = MODE12_AG | MODE12_GM1 | MODE12_GM0 | MODE12_CSS;
out_port(PORT_40, mode_byte);
```

#### Optimized Block Drawing
- **1 bit per pixel**: 8 pixels per byte, perfect alignment
- **8x8 blocks**: Exactly 8 memory writes per block
- **Fast addressing**: `addr = VRAM_START + ((y + row) << 5) + (x >> 3)`

#### Block Patterns
1. **Filled Blocks**: `POKE(addr, 0xFF)` for solid appearance
2. **Outline Blocks**: Border pattern for visual separation
   ```c
   // Top/bottom: 0xFF (11111111)
   // Sides:     0x81 (10000001)
   ```

### Screen Layout Design
```
256x192 Total Screen
┌─────────────────────────────────────────────────────────┐
│ Title Area (8 pixels)                                   │
├──────────┬─────────────────────┬─────────────────────────┤
│ Left UI  │ Playfield (80x160)  │ Right UI               │
│ (72px)   │ 10x20 blocks        │ (72px)                 │
│          │ Official Tetris     │                        │
│          │ dimensions          │                        │
└──────────┴─────────────────────┴─────────────────────────┘
```

### Key Achievements
1. **Professional Appearance**: Crisp, well-proportioned blocks
2. **Standard Dimensions**: Official 10x20 Tetris playfield
3. **Visual Clarity**: Clear block separation with outline patterns
4. **Performance**: Optimized for real-time gameplay
5. **Authentic Feel**: Classic monochrome Tetris aesthetic

## Technical Lessons Learned

### Z88dk Integration
- **Custom CRT0**: Successfully implemented minimal startup code
- **I/O Port Access**: Critical to use `OUT` instruction instead of memory writes
- **Inline Assembly**: Required for hardware port access
- **Build System**: z88dk +sos target works well for PHC25

### Graphics Optimization
- **Memory Alignment**: 4-pixel boundaries crucial for Mode 11
- **Bit Manipulation**: Efficient pixel operations using shifts and masks
- **Address Calculation**: Bit shifts faster than multiplication/division
- **Pattern Design**: Simple patterns perform better than complex calculations

### Hardware Insights
- **Port $40**: Controls MC6847 graphics modes via I/O port (not memory)
- **VRAM Layout**: $6000-$77FF, row addressing with bit shifts
- **Mode Selection**: Mode 12 superior for block-based games
- **Aspect Ratio**: Higher resolution compensates for pixel stretching

## Recommended Implementation Strategy

### Core Graphics System
1. **Use Mode 12** (256x192 monochrome) as established in POC 2
2. **8x8 pixel blocks** for optimal visual quality
3. **Outline block patterns** for piece differentiation
4. **Optimized drawing functions** for real-time performance

### Platform Integration
1. **Adapt existing platform abstraction** from Alice version
2. **Map character-based API** to pixel-based Mode 12 functions
3. **Implement display sync** for efficient VRAM updates
4. **Add keyboard input** using PHC25 matrix scanning

### Performance Considerations
1. **Block-based updates**: Only redraw changed blocks
2. **Dirty region tracking**: Minimize VRAM writes
3. **Fast line clearing**: Optimized row operations
4. **Efficient piece movement**: Block copying vs individual redraws

### Future Enhancements
1. **Pattern Variations**: Different interior patterns to simulate colors
2. **Animation Effects**: Line clear animations using block patterns
3. **Sound Integration**: AY8910 chip for audio feedback
4. **Enhanced UI**: Score display using character patterns

## File Structure for Full Implementation

```
phc25_tetris/
├── src/
│   ├── main.c              # Main game loop
│   ├── platform_phc25.c    # Platform implementation
│   ├── crt0.asm           # Startup code
│   └── graphics.c          # Mode 12 graphics functions
├── include/
│   ├── phc25.h            # Hardware definitions
│   └── graphics.h         # Graphics function prototypes
└── Makefile               # z88dk build configuration
```

## Conclusions

The POC development successfully identified Mode 12 (256x192 monochrome) as the optimal graphics approach for PHC25 Tetris. The higher resolution provides professional visual quality while maintaining excellent performance. The outline block pattern creates clear visual separation between pieces while allowing for future pattern-based color simulation.

Key success factors:
- **Resolution over color**: 256x192 monochrome superior to 128x192 4-color
- **Block alignment**: 8x8 blocks provide perfect proportions
- **Hardware optimization**: Direct I/O port access and efficient memory operations
- **Standard layout**: Official 10x20 Tetris playfield with UI space

The foundation is now ready for full Tetris implementation using the established Mode 12 graphics system and optimized block drawing functions.