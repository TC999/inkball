// Ball physics and collision detection based on IDA decompilation

use crate::ball::Ball;
use crate::board::{GameBoard, TILE_SIZE};
use crate::tile::{BoardTile, TileType, Direction};

const GRAVITY: f64 = 0.15;  // Gravity acceleration
const FRICTION: f64 = 0.99; // Velocity damping
const BOUNCE_FACTOR: f64 = 0.8; // Energy loss on bounce

pub struct Physics;

impl Physics {
    /// Update ball position based on velocity
    /// delta_time should be in seconds for physics calculations
    pub fn update_ball_position(ball: &mut Ball, delta_time: f64) {
        if !ball.alive || ball.captured {
            return;
        }
        
        // Apply gravity (scaled by delta_time for frame-rate independence)
        ball.vy += GRAVITY * delta_time;
        
        // Apply friction (frame-rate independent using power function)
        let friction_factor = FRICTION.powf(delta_time);
        ball.vx *= friction_factor;
        ball.vy *= friction_factor;
        
        // Update position
        ball.x += ball.vx * delta_time;
        ball.y += ball.vy * delta_time;
        
        // Update grid position
        ball.row = (ball.y / TILE_SIZE as f64) as usize;
        ball.col = (ball.x / TILE_SIZE as f64) as usize;
    }
    
    /// Check and handle collision with board bounds
    pub fn check_board_bounds(ball: &mut Ball, board: &GameBoard) -> bool {
        let board_width = board.tiles[0].len() as f64 * TILE_SIZE as f64;
        let board_height = board.tiles.len() as f64 * TILE_SIZE as f64;
        
        let mut collided = false;
        
        // Left/Right bounds
        if ball.x - ball.radius < 0.0 {
            ball.x = ball.radius;
            ball.vx = -ball.vx * BOUNCE_FACTOR;
            collided = true;
        } else if ball.x + ball.radius > board_width {
            ball.x = board_width - ball.radius;
            ball.vx = -ball.vx * BOUNCE_FACTOR;
            collided = true;
        }
        
        // Top/Bottom bounds
        if ball.y - ball.radius < 0.0 {
            ball.y = ball.radius;
            ball.vy = -ball.vy * BOUNCE_FACTOR;
            collided = true;
        } else if ball.y + ball.radius > board_height {
            ball.y = board_height - ball.radius;
            ball.vy = -ball.vy * BOUNCE_FACTOR;
            collided = true;
        }
        
        collided
    }
    
    /// Check collision with tiles and deflect
    pub fn check_tile_collision(ball: &mut Ball, board: &GameBoard) -> bool {
        let mut collided = false;
        
        // Get tiles around the ball
        let min_row = ((ball.y - ball.radius) / TILE_SIZE as f64).max(0.0) as usize;
        let max_row = ((ball.y + ball.radius) / TILE_SIZE as f64).min((board.tiles.len() - 1) as f64) as usize;
        let min_col = ((ball.x - ball.radius) / TILE_SIZE as f64).max(0.0) as usize;
        let max_col = ((ball.x + ball.radius) / TILE_SIZE as f64).min((board.tiles[0].len() - 1) as f64) as usize;
        
        for row in min_row..=max_row {
            for col in min_col..=max_col {
                if let Some(tile) = board.get_tile(row, col) {
                    if Self::check_single_tile_collision(ball, tile) {
                        collided = true;
                    }
                }
            }
        }
        
        collided
    }
    
    /// Check collision with a single tile
    fn check_single_tile_collision(ball: &mut Ball, tile: &BoardTile) -> bool {
        match tile.tile_type {
            TileType::Floor => false,
            TileType::Wall | TileType::BreakWall => {
                Self::deflect_from_wall(ball, tile)
            }
            TileType::Bumper => {
                Self::deflect_from_bumper(ball, tile)
            }
            TileType::Chevron(dir) => {
                Self::deflect_from_chevron(ball, tile, dir)
            }
            TileType::Drain(color) => {
                Self::check_drain_capture(ball, tile, color)
            }
            TileType::RLColored(color) => {
                Self::deflect_from_rl_colored(ball, tile, color)
            }
            TileType::RLGray => {
                Self::deflect_from_rl_gray(ball, tile)
            }
            TileType::OneWayForce(dir) => {
                Self::deflect_from_owf(ball, tile, dir)
            }
        }
    }
    
    /// Deflect ball from a wall
    fn deflect_from_wall(ball: &mut Ball, tile: &BoardTile) -> bool {
        let tile_x = tile.col as f64 * TILE_SIZE as f64;
        let tile_y = tile.row as f64 * TILE_SIZE as f64;
        let tile_center_x = tile_x + TILE_SIZE as f64 / 2.0;
        let tile_center_y = tile_y + TILE_SIZE as f64 / 2.0;
        
        // Check if ball overlaps with tile
        let closest_x = ball.x.max(tile_x).min(tile_x + TILE_SIZE as f64);
        let closest_y = ball.y.max(tile_y).min(tile_y + TILE_SIZE as f64);
        
        let dx = ball.x - closest_x;
        let dy = ball.y - closest_y;
        let dist_sq = dx * dx + dy * dy;
        
        if dist_sq < ball.radius * ball.radius {
            // Determine which side was hit
            let from_left = ball.x < tile_center_x;
            let from_top = ball.y < tile_center_y;
            
            let dx_abs = (ball.x - tile_center_x).abs();
            let dy_abs = (ball.y - tile_center_y).abs();
            
            if dx_abs > dy_abs {
                // Hit from left or right
                ball.vx = -ball.vx * BOUNCE_FACTOR;
                if from_left {
                    ball.x = tile_x - ball.radius;
                } else {
                    ball.x = tile_x + TILE_SIZE as f64 + ball.radius;
                }
            } else {
                // Hit from top or bottom
                ball.vy = -ball.vy * BOUNCE_FACTOR;
                if from_top {
                    ball.y = tile_y - ball.radius;
                } else {
                    ball.y = tile_y + TILE_SIZE as f64 + ball.radius;
                }
            }
            
            return true;
        }
        
        false
    }
    
    /// Deflect ball from a bumper (random direction)
    fn deflect_from_bumper(ball: &mut Ball, tile: &BoardTile) -> bool {
        let tile_x = tile.col as f64 * TILE_SIZE as f64 + TILE_SIZE as f64 / 2.0;
        let tile_y = tile.row as f64 * TILE_SIZE as f64 + TILE_SIZE as f64 / 2.0;
        
        let dx = ball.x - tile_x;
        let dy = ball.y - tile_y;
        let dist = (dx * dx + dy * dy).sqrt();
        
        if dist < ball.radius + TILE_SIZE as f64 / 2.0 {
            // Bounce in opposite direction with increased velocity
            let angle = dy.atan2(dx);
            let speed = (ball.vx * ball.vx + ball.vy * ball.vy).sqrt() * 1.2;
            ball.vx = angle.cos() * speed;
            ball.vy = angle.sin() * speed;
            
            // Move ball away from bumper
            let push_dist = ball.radius + TILE_SIZE as f64 / 2.0;
            ball.x = tile_x + angle.cos() * push_dist;
            ball.y = tile_y + angle.sin() * push_dist;
            
            return true;
        }
        
        false
    }
    
    /// Deflect ball from a chevron (directional deflector)
    fn deflect_from_chevron(ball: &mut Ball, tile: &BoardTile, direction: Direction) -> bool {
        if Self::deflect_from_wall(ball, tile) {
            // Modify deflection based on chevron direction
            match direction {
                Direction::Up => ball.vy = -ball.vy.abs(),
                Direction::Down => ball.vy = ball.vy.abs(),
                Direction::Left => ball.vx = -ball.vx.abs(),
                Direction::Right => ball.vx = ball.vx.abs(),
                _ => {}
            }
            return true;
        }
        false
    }
    
    /// Check if ball is captured by drain
    fn check_drain_capture(ball: &mut Ball, tile: &BoardTile, drain_color: crate::tile::BoardColor) -> bool {
        let tile_x = tile.col as f64 * TILE_SIZE as f64 + TILE_SIZE as f64 / 2.0;
        let tile_y = tile.row as f64 * TILE_SIZE as f64 + TILE_SIZE as f64 / 2.0;
        
        let dx = ball.x - tile_x;
        let dy = ball.y - tile_y;
        let dist = (dx * dx + dy * dy).sqrt();
        
        if dist < TILE_SIZE as f64 / 4.0 {
            // Check if ball color matches drain color
            let ball_matches = match (ball.color, drain_color) {
                (crate::ball::BallColor::Red, crate::tile::BoardColor::Red) => true,
                (crate::ball::BallColor::Blue, crate::tile::BoardColor::Blue) => true,
                (crate::ball::BallColor::Green, crate::tile::BoardColor::Green) => true,
                (crate::ball::BallColor::Yellow, crate::tile::BoardColor::Yellow) => true,
                (crate::ball::BallColor::Cyan, crate::tile::BoardColor::Cyan) => true,
                (crate::ball::BallColor::Gray, crate::tile::BoardColor::Gray) => true,
                _ => false,
            };
            
            if ball_matches {
                ball.captured = true;
                ball.alive = false;
                return true;
            }
        }
        
        false
    }
    
    /// Deflect from colored reflective wall
    fn deflect_from_rl_colored(ball: &mut Ball, tile: &BoardTile, _color: crate::tile::BoardColor) -> bool {
        // Colored walls behave like regular walls
        Self::deflect_from_wall(ball, tile)
    }
    
    /// Deflect from gray reflective wall (opens periodically)
    fn deflect_from_rl_gray(ball: &mut Ball, tile: &BoardTile) -> bool {
        // Note: Gray reflective walls should open and close periodically in the original game
        // This requires game time tracking to implement properly
        // For now, treating as a regular wall until time management is implemented
        Self::deflect_from_wall(ball, tile)
    }
    
    /// Deflect from one-way force field
    fn deflect_from_owf(ball: &mut Ball, tile: &BoardTile, direction: Direction) -> bool {
        // One-way force fields allow balls to pass in one direction but block in the opposite
        // Check if ball is moving against the allowed direction
        let should_block = match direction {
            Direction::Up => ball.vy > 0.0,      // Block downward movement
            Direction::Down => ball.vy < 0.0,    // Block upward movement  
            Direction::Left => ball.vx > 0.0,    // Block rightward movement
            Direction::Right => ball.vx < 0.0,   // Block leftward movement
            _ => false,  // Diagonal directions not typically used for OWF
        };
        
        if should_block {
            Self::deflect_from_wall(ball, tile)
        } else {
            // Allow passage
            false
        }
    }
}
