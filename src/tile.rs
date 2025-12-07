// Tile types based on IDA decompilation

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TileType {
    Floor,
    Wall,
    BreakWall,       // Wall that breaks when hit
    Bumper,          // Bounces balls in random direction
    Chevron(Direction),  // Deflects balls in specific direction
    Drain(BoardColor),   // Captures balls of matching color
    RLColored(BoardColor),  // Red/Blue/Green/Yellow/Cyan colored reflective walls
    RLGray,          // Gray reflective wall (opens periodically)
    OneWayForce(Direction),     // One-way force field (allows passage in specified direction)
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Direction {
    Up,
    Down,
    Left,
    Right,
    UpLeft,
    UpRight,
    DownLeft,
    DownRight,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum BoardColor {
    Red,
    Blue,
    Green,
    Yellow,
    Cyan,
    Gray,
}

impl BoardColor {
    pub fn to_fltk_color(&self) -> fltk::enums::Color {
        use fltk::enums::Color;
        match self {
            BoardColor::Red => Color::Red,
            BoardColor::Blue => Color::Blue,
            BoardColor::Green => Color::Green,
            BoardColor::Yellow => Color::Yellow,
            BoardColor::Cyan => Color::Cyan,
            BoardColor::Gray => Color::from_rgb(128, 128, 128),
        }
    }
}

/// Represents a tile on the game board
#[derive(Debug, Clone, Copy)]
pub struct BoardTile {
    pub row: usize,
    pub col: usize,
    pub tile_type: TileType,
    pub shadowed: bool,      // Whether this tile is shadowed
    pub closest_side: u8,    // Which side was hit last (for collision)
}

impl BoardTile {
    pub fn new(row: usize, col: usize, tile_type: TileType) -> Self {
        Self {
            row,
            col,
            tile_type,
            shadowed: false,
            closest_side: 0,
        }
    }
    
    /// Check if a ball should deflect off this tile
    pub fn should_deflect(&self) -> bool {
        matches!(self.tile_type,
            TileType::Wall | TileType::BreakWall | TileType::Bumper |
            TileType::RLColored(_) | TileType::RLGray | TileType::Chevron(_))
    }
    
    /// Check if this tile can capture balls
    pub fn can_capture(&self) -> bool {
        matches!(self.tile_type, TileType::Drain(_))
    }
    
    /// Get the tile's board color if applicable
    pub fn get_color(&self) -> Option<BoardColor> {
        match self.tile_type {
            TileType::Drain(color) | TileType::RLColored(color) => Some(color),
            _ => None,
        }
    }
}
