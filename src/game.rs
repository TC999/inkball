use crate::board::GameBoard;
use crate::ball::{BallColor};

pub struct Game {
    pub board: GameBoard,
}

impl Game {
    pub fn new() -> Self {
        Self {
            board: GameBoard::new(),
        }
    }

    pub fn handle_click(&mut self, x: i32, y: i32, tile_size: i32) {
        let row = (y / tile_size) as usize;
        let col = (x / tile_size) as usize;
        let color = match (row + col) % 3 {
            0 => BallColor::Red,
            1 => BallColor::Blue,
            _ => BallColor::Green,
        };
        self.board.add_ball(row, col, color);
    }
}
