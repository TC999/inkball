use crate::ball::{Ball, BallColor};
use crate::tile::{BoardTile, TileType};

pub const BOARD_ROWS: usize = 17;
pub const BOARD_COLS: usize = 17;
pub const TILE_SIZE: i32 = 48;

pub struct GameBoard {
    pub tiles: [[Option<BoardTile>; BOARD_COLS]; BOARD_ROWS],
    pub balls: Vec<Ball>,
    pub entry_pos: (usize, usize), // 出球口
    pub goal_pos: (usize, usize),  // 进球口
}

impl GameBoard {
                /// 判断是否胜利（有球进入进球口）
                pub fn is_win(&self) -> bool {
                    self.balls.iter().any(|ball| ball.captured)
                }
            /// 鼠标画墙，将指定格子设为 BreakWall
            pub fn draw_wall(&mut self, row: usize, col: usize) {
                if row > 0 && row < BOARD_ROWS-1 && col > 0 && col < BOARD_COLS-1 {
                    if let Some(tile) = &mut self.tiles[row][col] {
                        if matches!(tile.tile_type, TileType::Floor) {
                            tile.tile_type = TileType::BreakWall;
                            tile.hit_count = 0;
                        }
                    }
                }
            }
        /// 更新所有球的位置和碰撞
        pub fn update_balls(&mut self) {
            for ball in &mut self.balls {
                if !ball.alive || ball.captured {
                    continue;
                }
                // 移动球
                ball.x += ball.vx;
                ball.y += ball.vy;

                // 计算球当前所在格子
                let col = (ball.x / TILE_SIZE as f64).floor() as usize;
                let row = (ball.y / TILE_SIZE as f64).floor() as usize;
                ball.row = row;
                ball.col = col;

                // 检查是否碰到墙或 BreakWall
                if row < BOARD_ROWS && col < BOARD_COLS {
                    if let Some(tile) = &mut self.tiles[row][col] {
                        match tile.tile_type {
                            TileType::Wall => {
                                // 反弹：简单处理，速度取反
                                let left = (ball.x - ball.radius) < (col as f64 * TILE_SIZE as f64);
                                let right = (ball.x + ball.radius) > ((col+1) as f64 * TILE_SIZE as f64);
                                let top = (ball.y - ball.radius) < (row as f64 * TILE_SIZE as f64);
                                let bottom = (ball.y + ball.radius) > ((row+1) as f64 * TILE_SIZE as f64);
                                if left || right {
                                    ball.vx = -ball.vx;
                                }
                                if top || bottom {
                                    ball.vy = -ball.vy;
                                }
                            },
                            TileType::BreakWall => {
                                // 反弹并计数，碰一次后消失
                                let left = (ball.x - ball.radius) < (col as f64 * TILE_SIZE as f64);
                                let right = (ball.x + ball.radius) > ((col+1) as f64 * TILE_SIZE as f64);
                                let top = (ball.y - ball.radius) < (row as f64 * TILE_SIZE as f64);
                                let bottom = (ball.y + ball.radius) > ((row+1) as f64 * TILE_SIZE as f64);
                                if left || right {
                                    ball.vx = -ball.vx;
                                }
                                if top || bottom {
                                    ball.vy = -ball.vy;
                                }
                                tile.hit_count += 1;
                                if tile.hit_count >= 1 {
                                    tile.tile_type = TileType::Floor;
                                    tile.hit_count = 0;
                                }
                            },
                            TileType::Drain(_) => {
                                // 进球口，判定胜利
                                ball.captured = true;
                                ball.alive = false;
                            },
                            _ => {}
                        }
                    }
                }
            }
        }
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
        
        use rand::Rng;
        let mut rng = rand::thread_rng();
        // 随机选取出球口和进球口，不能在最外围
        let entry_pos = loop {
            let row = rng.gen_range(1..BOARD_ROWS-1);
            let col = rng.gen_range(1..BOARD_COLS-1);
            if tiles[row][col].as_ref().unwrap().tile_type == TileType::Floor {
                break (row, col);
            }
        };
        let goal_pos = loop {
            let row = rng.gen_range(1..BOARD_ROWS-1);
            let col = rng.gen_range(1..BOARD_COLS-1);
            if (row, col) != entry_pos && tiles[row][col].as_ref().unwrap().tile_type == TileType::Floor {
                break (row, col);
            }
        };
        // 设置进球口为 Drain
        tiles[goal_pos.0][goal_pos.1] = Some(BoardTile::new(goal_pos.0, goal_pos.1, TileType::Drain(crate::tile::BoardColor::Gray)));

        let mut board = Self {
            tiles,
            balls: Vec::new(),
            entry_pos,
            goal_pos,
        };
        // 游戏初始化时自动在出球口生成球
        board.add_ball(entry_pos.0, entry_pos.1, BallColor::Gray);
        board
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
