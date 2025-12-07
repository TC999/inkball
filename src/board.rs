use crate::ball::{Ball, BallColor};

pub const BOARD_ROWS: usize = 9;
pub const BOARD_COLS: usize = 9;

pub struct GameBoard {
    pub tiles: [[Option<Ball>; BOARD_COLS]; BOARD_ROWS],
}

impl GameBoard {
    pub fn new() -> Self {
        Self {
            tiles: [[None; BOARD_COLS]; BOARD_ROWS],
        }
    }

    pub fn add_ball(&mut self, row: usize, col: usize, color: BallColor) {
        if row < BOARD_ROWS && col < BOARD_COLS && self.tiles[row][col].is_none() {
            self.tiles[row][col] = Some(Ball { row, col, color });
        }
    }
}
