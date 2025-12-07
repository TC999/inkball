#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum BallColor {
    Red,
    Blue,
    Green,
    Yellow,
    Cyan,
    Gray,
}

impl BallColor {
    pub fn to_fltk_color(&self) -> fltk::enums::Color {
        use fltk::enums::Color;
        match self {
            BallColor::Red => Color::Red,
            BallColor::Blue => Color::Blue,
            BallColor::Green => Color::Green,
            BallColor::Yellow => Color::Yellow,
            BallColor::Cyan => Color::Cyan,
            BallColor::Gray => Color::from_rgb(128, 128, 128),
        }
    }
}

/// Ball point - position and velocity in the ball's trajectory
#[derive(Clone, Copy, Debug)]
pub struct BallPoint {
    pub x: i32,
    pub y: i32,
    pub vx: i32,  // Velocity in x direction
    pub vy: i32,  // Velocity in y direction
}

#[derive(Clone, Copy, Debug)]
pub struct Ball {
    pub row: usize,
    pub col: usize,
    pub color: BallColor,
    pub x: f64,           // Pixel position x
    pub y: f64,           // Pixel position y
    pub vx: f64,          // Velocity x
    pub vy: f64,          // Velocity y
    pub radius: f64,      // Ball radius
    pub alive: bool,      // Is the ball still in play
    pub captured: bool,   // Has the ball been captured by a drain
}
