use crate::board::GameBoard;
use crate::ball::BallColor;
use crate::physics::Physics;

pub struct Game {
    pub board: GameBoard,
    pub score: u32,
    pub time_remaining: u32,
    pub level: u32,
    pub paused: bool,
}

impl Game {
    pub fn new() -> Self {
        Self {
            board: GameBoard::new(),
            score: 0,
            time_remaining: 300, // 5 minutes
            level: 1,
            paused: false,
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
        
        // Add ball with initial velocity for testing
        self.board.add_ball(row, col, color);
        
        // Give the ball a random initial velocity
        if let Some(ball) = self.board.balls.last_mut() {
            ball.vx = (((row + col) % 5) as f64 - 2.0) * 2.0;
            ball.vy = -3.0; // Start moving upward
        }
    }
    
    pub fn update(&mut self, delta_time: f64) {
        if self.paused {
            return;
        }
        
        // Get board dimensions for collision checking
        let board_rows = self.board.tiles.len();
        let board_cols = if board_rows > 0 { self.board.tiles[0].len() } else { 0 };
        
        // Update all balls
        for ball in &mut self.board.balls {
            if ball.alive && !ball.captured {
                // Update position with physics
                Physics::update_ball_position(ball, delta_time);
                
                // Check collisions with board bounds (inline to avoid borrow issues)
                let board_width = board_cols as f64 * crate::board::TILE_SIZE as f64;
                let board_height = board_rows as f64 * crate::board::TILE_SIZE as f64;
                
                if ball.x - ball.radius < 0.0 {
                    ball.x = ball.radius;
                    ball.vx = -ball.vx * 0.8;
                } else if ball.x + ball.radius > board_width {
                    ball.x = board_width - ball.radius;
                    ball.vx = -ball.vx * 0.8;
                }
                
                if ball.y - ball.radius < 0.0 {
                    ball.y = ball.radius;
                    ball.vy = -ball.vy * 0.8;
                } else if ball.y + ball.radius > board_height {
                    ball.y = board_height - ball.radius;
                    ball.vy = -ball.vy * 0.8;
                }
                
                // Check collisions with tiles
                let tile_row = (ball.y / crate::board::TILE_SIZE as f64) as usize;
                let tile_col = (ball.x / crate::board::TILE_SIZE as f64) as usize;
                
                if tile_row < board_rows && tile_col < board_cols {
                    if let Some(tile) = self.board.tiles[tile_row][tile_col] {
                        // Simple collision - just check if we're in a wall tile
                        if matches!(tile.tile_type, crate::tile::TileType::Wall) {
                            // Simple bounce
                            ball.vx = -ball.vx * 0.8;
                            ball.vy = -ball.vy * 0.8;
                        }
                    }
                }
            }
        }
        
        // Remove captured or dead balls
        self.board.balls.retain(|ball| ball.alive);
        
        // TODO: Update time and check win/loss conditions
        // TODO: Implement ink collision detection
    }
    
    pub fn toggle_pause(&mut self) {
        self.paused = !self.paused;
    }
}
