use crate::ball::{Ball, BallColor};
use crate::tile::{BoardTile, TileType};

pub const BOARD_ROWS: usize = 9;
pub const BOARD_COLS: usize = 9;
pub const TILE_SIZE: i32 = 48;

pub struct GameBoard {
    pub tiles: [[Option<BoardTile>; BOARD_COLS]; BOARD_ROWS],
    pub balls: Vec<Ball>,
}

impl GameBoard {
    pub fn new() -> Self {
        let mut tiles = [[None; BOARD_COLS]; BOARD_ROWS];
        
        // Initialize with floor tiles
        for row in 0..BOARD_ROWS {
            for col in 0..BOARD_COLS {
                tiles[row][col] = Some(BoardTile::new(row, col, TileType::Floor));
            }
        }
        
        // Add border walls
        for col in 0..BOARD_COLS {
            tiles[0][col] = Some(BoardTile::new(0, col, TileType::Wall));
            tiles[BOARD_ROWS - 1][col] = Some(BoardTile::new(BOARD_ROWS - 1, col, TileType::Wall));
        }
        for row in 0..BOARD_ROWS {
            tiles[row][0] = Some(BoardTile::new(row, 0, TileType::Wall));
            tiles[row][BOARD_COLS - 1] = Some(BoardTile::new(row, BOARD_COLS - 1, TileType::Wall));
        }
        
        Self {
            tiles,
            balls: Vec::new(),
        }
    }

    pub fn add_ball(&mut self, row: usize, col: usize, color: BallColor) {
        if row < BOARD_ROWS && col < BOARD_COLS {
            let x = (col as f64 * TILE_SIZE as f64) + (TILE_SIZE as f64 / 2.0);
            let y = (row as f64 * TILE_SIZE as f64) + (TILE_SIZE as f64 / 2.0);
            
            self.balls.push(Ball {
                row,
                col,
                color,
                x,
                y,
                vx: 0.0,
                vy: 0.0,
                radius: TILE_SIZE as f64 / 2.5,
                alive: true,
                captured: false,
            });
        }
    }
    
    pub fn get_tile(&self, row: usize, col: usize) -> Option<&BoardTile> {
        if row < BOARD_ROWS && col < BOARD_COLS {
            self.tiles[row][col].as_ref()
        } else {
            None
        }
    }
    
    pub fn set_tile(&mut self, row: usize, col: usize, tile: BoardTile) {
        if row < BOARD_ROWS && col < BOARD_COLS {
            self.tiles[row][col] = Some(tile);
        }
    }
}
