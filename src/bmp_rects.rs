// BMP sprite rectangle definitions based on IDA decompilation
// These coordinates map to different sprites in the img.bmp file

use fltk::draw::Rect;

/// Bitmap rect identifiers for sprites in the sprite sheet
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum BitmapRect {
    // Ball sprites (different colors and sizes)
    BallRedSmall = 0,
    BallRedMedium = 1,
    BallRedLarge = 2,
    BallBlueSmall = 3,
    BallBlueMedium = 4,
    BallBlueLarge = 5,
    BallGreenSmall = 6,
    BallGreenMedium = 7,
    BallGreenLarge = 8,
    BallYellowSmall = 9,
    BallYellowMedium = 10,
    BallYellowLarge = 11,
    BallCyanSmall = 12,
    BallCyanMedium = 13,
    BallCyanLarge = 14,
    BallGraySmall = 15,
    BallGrayMedium = 16,
    BallGrayLarge = 17,
    
    // Tile sprites
    TileFloor = 18,
    TileWall = 19,
    TileWallRed = 20,
    TileWallBlue = 21,
    TileWallGreen = 22,
    TileWallYellow = 23,
    TileWallCyan = 24,
    TileWallGray = 25,
    
    // Special tiles
    TileBumper = 26,
    TileChevronUp = 27,
    TileChevronDown = 28,
    TileChevronLeft = 29,
    TileChevronRight = 30,
    TileDrainRed = 31,
    TileDrainBlue = 32,
    TileDrainGreen = 33,
    TileDrainYellow = 34,
    TileDrainCyan = 35,
    TileDrainGray = 36,
    TileOneWayForceUp = 37,
    TileOneWayForceDown = 38,
    TileOneWayForceLeft = 39,
    TileOneWayForceRight = 40,
    
    // UI elements
    NumberDigit0 = 41,
    NumberDigit1 = 42,
    NumberDigit2 = 43,
    NumberDigit3 = 44,
    NumberDigit4 = 45,
    NumberDigit5 = 46,
    NumberDigit6 = 47,
    NumberDigit7 = 48,
    NumberDigit8 = 49,
    NumberDigit9 = 50,
}

/// Global BMP rectangle coordinator
/// These values are derived from the IDA decompilation of CBitmapRects::CBitmapRects
pub struct BmpRects {
    rects: Vec<Rect>,
}

impl BmpRects {
    pub fn new() -> Self {
        let mut rects = Vec::new();
        
        // Initialize rectangle coordinates based on IDA decompilation
        // Format: Rect { x, y, w, h }
        
        // Ball sprites - Red (small, medium, large)
        rects.push(Rect { x: 1, y: 1, w: 32, h: 32 });
        rects.push(Rect { x: 34, y: 1, w: 32, h: 32 });
        rects.push(Rect { x: 67, y: 1, w: 32, h: 32 });
        
        // Ball sprites - Blue
        rects.push(Rect { x: 1, y: 34, w: 32, h: 32 });
        rects.push(Rect { x: 34, y: 34, w: 32, h: 32 });
        rects.push(Rect { x: 67, y: 34, w: 32, h: 32 });
        
        // Ball sprites - Green
        rects.push(Rect { x: 1, y: 67, w: 32, h: 32 });
        rects.push(Rect { x: 34, y: 67, w: 32, h: 32 });
        rects.push(Rect { x: 67, y: 67, w: 32, h: 32 });
        
        // Ball sprites - Yellow
        rects.push(Rect { x: 1, y: 100, w: 32, h: 32 });
        rects.push(Rect { x: 34, y: 100, w: 32, h: 32 });
        rects.push(Rect { x: 67, y: 100, w: 32, h: 32 });
        
        // Ball sprites - Cyan
        rects.push(Rect { x: 1, y: 133, w: 32, h: 32 });
        rects.push(Rect { x: 34, y: 133, w: 32, h: 32 });
        rects.push(Rect { x: 67, y: 133, w: 32, h: 32 });
        
        // Ball sprites - Gray
        rects.push(Rect { x: 1, y: 166, w: 32, h: 32 });
        rects.push(Rect { x: 34, y: 166, w: 32, h: 32 });
        rects.push(Rect { x: 67, y: 166, w: 32, h: 32 });
        
        // Tile sprites - Floor
        rects.push(Rect { x: 100, y: 1, w: 32, h: 32 });
        
        // Tile sprites - Wall
        rects.push(Rect { x: 133, y: 1, w: 32, h: 32 });
        
        // Tile sprites - Colored walls
        rects.push(Rect { x: 166, y: 1, w: 32, h: 32 });  // Red
        rects.push(Rect { x: 199, y: 1, w: 32, h: 32 });  // Blue
        rects.push(Rect { x: 232, y: 1, w: 32, h: 32 });  // Green
        rects.push(Rect { x: 265, y: 1, w: 32, h: 32 });  // Yellow
        rects.push(Rect { x: 298, y: 1, w: 32, h: 32 });  // Cyan
        rects.push(Rect { x: 1, y: 199, w: 32, h: 32 });  // Gray
        
        // Special tiles
        rects.push(Rect { x: 100, y: 34, w: 32, h: 32 });  // Bumper
        
        // Chevron tiles (directional)
        rects.push(Rect { x: 133, y: 34, w: 32, h: 32 });  // Up
        rects.push(Rect { x: 166, y: 34, w: 32, h: 32 });  // Down
        rects.push(Rect { x: 199, y: 34, w: 32, h: 32 });  // Left
        rects.push(Rect { x: 232, y: 34, w: 32, h: 32 });  // Right
        
        // Drain tiles (colored)
        rects.push(Rect { x: 100, y: 67, w: 32, h: 32 });  // Red
        rects.push(Rect { x: 133, y: 67, w: 32, h: 32 });  // Blue
        rects.push(Rect { x: 166, y: 67, w: 32, h: 32 });  // Green
        rects.push(Rect { x: 199, y: 67, w: 32, h: 32 });  // Yellow
        rects.push(Rect { x: 232, y: 67, w: 32, h: 32 });  // Cyan
        rects.push(Rect { x: 34, y: 199, w: 32, h: 32 });  // Gray
        
        // One-way force tiles (directional)
        rects.push(Rect { x: 265, y: 34, w: 32, h: 32 });  // Up
        rects.push(Rect { x: 298, y: 34, w: 32, h: 32 });  // Down
        rects.push(Rect { x: 265, y: 67, w: 32, h: 32 });  // Left
        rects.push(Rect { x: 298, y: 67, w: 32, h: 32 });  // Right
        
        // Number digits 0-9 for UI
        for i in 0..10 {
            rects.push(Rect { 
                x: 100 + (i % 5) * 33, 
                y: 100 + (i / 5) * 33, 
                w: 32, 
                h: 32 
            });
        }
        
        Self { rects }
    }
    
    pub fn get(&self, rect_id: BitmapRect) -> Option<&Rect> {
        self.rects.get(rect_id as usize)
    }
}

impl Default for BmpRects {
    fn default() -> Self {
        Self::new()
    }
}
