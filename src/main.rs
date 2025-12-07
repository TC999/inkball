mod ball;
mod board;
mod game;

use fltk::{
    app, image::BmpImage, prelude::*, window::Window, frame::Frame, draw, enums::Color,
};
use std::path::Path;
use game::Game;
use board::{BOARD_ROWS, BOARD_COLS};
use ball::{BallColor};

const TILE_SIZE: i32 = 48;

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

    let mut game = Game::new();

    frame.handle(move |f, ev| {
        if ev == fltk::enums::Event::Push {
            let (x, y) = (app::event_x(), app::event_y());
            game.handle_click(x, y, TILE_SIZE);
            f.redraw();
            true
        } else {
            false
        }
    });

    frame.draw(move |_f| {
        // 绘制棋盘格子
        for row in 0..BOARD_ROWS {
            for col in 0..BOARD_COLS {
                let x = col as i32 * TILE_SIZE;
                let y = row as i32 * TILE_SIZE;
                draw::draw_rect_fill(x, y, TILE_SIZE, TILE_SIZE, Color::White);
                draw::set_draw_color(Color::Black);
                draw::draw_rect(x, y, TILE_SIZE, TILE_SIZE);
                // 绘制球体
                if let Some(ball) = game.board.tiles[row][col] {
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
