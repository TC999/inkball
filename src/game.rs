use crate::board::GameBoard;
use crate::ball::BallColor;

pub struct Game {
    pub board: GameBoard,
    pub score: u32,
    pub time_remaining: u32,
    pub level: u32,
}

impl Game {
    pub fn new() -> Self {
        Self {
            board: GameBoard::new(),
            score: 0,
            time_remaining: 300, // 5 minutes
            level: 1,
        }
    }

    pub fn handle_click(&mut self, x: i32, y: i32, tile_size: i32) {
        let row = (y / tile_size) as usize;
        let col = (x / tile_size) as usize;
        
        // Cycle through colors on click for testing
        let color = match (row + col) % 6 {
            0 => BallColor::Red,
            1 => BallColor::Blue,
            2 => BallColor::Green,
            3 => BallColor::Yellow,
            4 => BallColor::Cyan,
            _ => BallColor::Gray,
        };
        
        self.board.add_ball(row, col, color);
    }
    
    pub fn update(&mut self, _delta_time: f64) {
        // TODO: Implement ball physics and collision detection
        // TODO: Update ball positions based on velocity
        // TODO: Check for collisions with tiles and ink
        // TODO: Update time and check win/loss conditions
    }
}
