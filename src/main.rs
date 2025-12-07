use fltk::{app, image::BmpImage, prelude::*, window::Window, draw, frame::Frame};
use std::path::Path;

fn main() {
    let app = app::App::default();
    let mut wind = Window::new(100, 100, 800, 600, "Inkball Rust FLTK");

    // 尝试加载 bmp 图像
    let bmp_path = Path::new("assets/img.bmp");
    let bmp_img = BmpImage::load(bmp_path);

    let mut frame = Frame::new(0, 0, 800, 600, None);
    if let Ok(img) = bmp_img {
        frame.set_image(Some(img));
    } else {
        frame.set_label("无法加载 assets/img.bmp");
    }

    wind.end();
    wind.show();
    app.run().unwrap();
}
