use fltk::{
    app, image::BmpImage, prelude::*, window::Window, frame::Frame, draw, enums::Color,
};
use std::path::Path;

const BOARD_ROWS: usize = 9;
const BOARD_COLS: usize = 9;
const TILE_SIZE: i32 = 48;

#[derive(Clone, Copy, PartialEq)]
enum BallColor {
    Red,
    Blue,
    Green,
}

struct Ball {
    row: usize,
    col: usize,
    color: BallColor,
}

struct GameBoard {
    tiles: [[Option<Ball>; BOARD_COLS]; BOARD_ROWS],
}

impl GameBoard {
    fn new() -> Self {
        Self {
            tiles: [[None; BOARD_COLS]; BOARD_ROWS],
        }
    }

    fn add_ball(&mut self, row: usize, col: usize, color: BallColor) {
        if row < BOARD_ROWS && col < BOARD_COLS && self.tiles[row][col].is_none() {
            self.tiles[row][col] = Some(Ball { row, col, color });
        }
    }
}

fn main() {
    let app = app::App::default();
    let mut wind = Window::new(
        100,
        100,
        BOARD_COLS as i32 * TILE_SIZE,
        BOARD_ROWS as i32 * TILE_SIZE,
        "Inkball Rust FLTK",
    );

    // 加载 bmp 图像
    let bmp_path = Path::new("assets/img.bmp");
    let bmp_img = BmpImage::load(bmp_path).ok();

    let mut frame = Frame::new(
        0,
        0,
        BOARD_COLS as i32 * TILE_SIZE,
        BOARD_ROWS as i32 * TILE_SIZE,
        None,
    );
    wind.end();
    wind.show();

    let mut board = GameBoard::new();

    frame.handle(move |f, ev| {
        if ev == fltk::enums::Event::Push {
            let (x, y) = (app::event_x(), app::event_y());
            let row = (y / TILE_SIZE) as usize;
            let col = (x / TILE_SIZE) as usize;
            // 随机颜色
            let color = match (row + col) % 3 {
                0 => BallColor::Red,
                1 => BallColor::Blue,
                _ => BallColor::Green,
            };
            board.add_ball(row, col, color);
            f.redraw();
            true
        } else {
            false
        }
    });

    frame.draw(move |f| {
        // 绘制棋盘格子
        for row in 0..BOARD_ROWS {
            for col in 0..BOARD_COLS {
                let x = col as i32 * TILE_SIZE;
                let y = row as i32 * TILE_SIZE;
                draw::set_draw_color(Color::White);
                draw::draw_rect_fill(x, y, TILE_SIZE, TILE_SIZE);
                draw::set_draw_color(Color::Black);
                draw::draw_rect(x, y, TILE_SIZE, TILE_SIZE);
                // 绘制球体
                if let Some(ball) = board.tiles[row][col] {
                    if let Some(img) = &bmp_img {
                        img.scale_tile(TILE_SIZE, TILE_SIZE, x, y);
                    } else {
                        let color = match ball.color {
                            BallColor::Red => Color::Red,
                            BallColor::Blue => Color::Blue,
                            BallColor::Green => Color::Green,
                        };
                        draw::set_draw_color(color);
                        draw::draw_circle(x + TILE_SIZE / 2, y + TILE_SIZE / 2, TILE_SIZE as f64 / 2.5);
                    }
                }
            }
        }
    });

    app.run().unwrap();
}
