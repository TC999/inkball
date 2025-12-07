mod ball;
mod board;
mod game;
mod tile;
mod bmp_rects;
mod physics;

use fltk::{
    app, image::BmpImage, prelude::*, window::Window, frame::Frame, draw, enums::Color,
};
use std::path::Path;
use std::rc::Rc;
use std::cell::RefCell;
use std::time::{Duration, Instant};
use game::Game;
use board::{BOARD_ROWS, BOARD_COLS, TILE_SIZE};

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

    let game_handle2 = Rc::clone(&game);
    frame.handle(move |f, ev| {
        match ev {
            fltk::enums::Event::Push => {
                let (x, y) = (app::event_x(), app::event_y());
                game_handle.borrow_mut().handle_click(x, y, TILE_SIZE);
                f.redraw();
                true
            }
            fltk::enums::Event::KeyDown => {
                let key = app::event_key();
                if key == fltk::enums::Key::from_char('p') || key == fltk::enums::Key::from_char('P') {
                    game_handle.borrow_mut().toggle_pause();
                    true
                } else {
                    false
                }
            }
            _ => false
        }
    });
    
    // Game update loop using timeout
    let last_update = Rc::new(RefCell::new(Instant::now()));
    let mut frame_clone = frame.clone();
    let last_update_clone = Rc::clone(&last_update);
    
    app::add_timeout3(0.016, move |handle| {
        let now = Instant::now();
        let delta_time = {
            let last = *last_update_clone.borrow();
            now.duration_since(last).as_secs_f64()
        };
        *last_update_clone.borrow_mut() = now;
        
        // Update game state
        game_handle2.borrow_mut().update(delta_time * 60.0); // Scale for 60 FPS
        
        // Trigger redraw
        frame_clone.redraw();
        
        // Schedule next update
        app::repeat_timeout3(0.016, handle);
    });

    frame.draw(move |_f| {
        // Draw tiles
        for row in 0..BOARD_ROWS {
            for col in 0..BOARD_COLS {
                let x = col as i32 * TILE_SIZE;
                let y = row as i32 * TILE_SIZE;
                
                // Draw tile background
                if let Some(tile) = game.borrow().board.get_tile(row, col) {
                    let tile_color = match tile.tile_type {
                        tile::TileType::Floor => Color::White,
                        tile::TileType::Wall => Color::from_rgb(64, 64, 64),
                        tile::TileType::BreakWall => Color::from_rgb(128, 128, 128),
                        _ => Color::from_rgb(96, 96, 96),
                    };
                    draw::draw_rect_fill(x, y, TILE_SIZE, TILE_SIZE, tile_color);
                } else {
                    draw::draw_rect_fill(x, y, TILE_SIZE, TILE_SIZE, Color::White);
                }
                
                draw::set_draw_color(Color::Black);
                draw::draw_rect(x, y, TILE_SIZE, TILE_SIZE);
            }
        }
        
        // Draw balls
        for ball in &game.borrow().board.balls {
            if ball.alive && !ball.captured {
                if let Some(_img_rc) = &bmp_img_handle {
                    // TODO: Draw ball sprite from BMP based on color and size
                    // For now, draw as colored circle
                    draw::set_draw_color(ball.color.to_fltk_color());
                } else {
                    draw::set_draw_color(ball.color.to_fltk_color());
                }
                
                let cx = ball.x as i32;
                let cy = ball.y as i32;
                let r = ball.radius;
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
    });

    app.run().unwrap();
}
