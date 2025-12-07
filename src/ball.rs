#[derive(Clone, Copy, PartialEq)]
pub enum BallColor {
    Red,
    Blue,
    Green,
}

#[derive(Clone, Copy)]
pub struct Ball {
    pub row: usize,
    pub col: usize,
    pub color: BallColor,
}
