mod ball;
mod board;
mod game;

use fltk::{
    app, image::BmpImage, prelude::*, window::Window, frame::Frame, draw, enums::Color,
};
use std::path::Path;
use std::rc::Rc;
use std::cell::RefCell;
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
    let bmp_img = bmp_img.map(|img| Rc::new(RefCell::new(img)));

    let mut frame = Frame::new(
        0,
        0,
        BOARD_COLS as i32 * TILE_SIZE,
        BOARD_ROWS as i32 * TILE_SIZE,
        None,
    );
    wind.end();
    wind.show();

    let game = Rc::new(RefCell::new(Game::new()));
    let game_handle = Rc::clone(&game);
    let bmp_img_handle = bmp_img.clone();

    frame.handle(move |f, ev| {
        if ev == fltk::enums::Event::Push {
            let (x, y) = (app::event_x(), app::event_y());
            game_handle.borrow_mut().handle_click(x, y, TILE_SIZE);
            f.redraw();
            true
        } else {
            false
        }
    });

    frame.draw(move |_f| {
        for row in 0..BOARD_ROWS {
            for col in 0..BOARD_COLS {
                let x = col as i32 * TILE_SIZE;
                let y = row as i32 * TILE_SIZE;
                draw::draw_rect_fill(x, y, TILE_SIZE, TILE_SIZE, Color::White);
                draw::set_draw_color(Color::Black);
                draw::draw_rect(x, y, TILE_SIZE, TILE_SIZE);
                // 绘制球体
                if let Some(ball) = game.borrow().board.tiles[row][col] {
                    if let Some(img_rc) = &bmp_img_handle {
                        img_rc.borrow_mut().draw(x, y, TILE_SIZE, TILE_SIZE);
                    } else {
                        let color = match ball.color {
                            BallColor::Red => Color::Red,
                            BallColor::Blue => Color::Blue,
                            BallColor::Green => Color::Green,
                        };
                        draw::set_draw_color(color);
                        let cx = x + TILE_SIZE / 2;
                        let cy = y + TILE_SIZE / 2;
                        let r = TILE_SIZE as f64 / 2.5;
                        draw::draw_pie(
                            (cx as f64 - r) as i32,
                            (cy as f64 - r) as i32,
                            (r * 2.0) as i32,
                            (r * 2.0) as i32,
                            0.0,
                            360.0,
                        );
                    }
                }
            }
        }
    });

    app.run().unwrap();
}
