pub struct Vec3 {
	pub x: f32,
	pub y: f32,
	pub z: f32,
}

#[repr(C)]
pub struct Mat4x4 {
	pub elements: [[f32; 4]; 4],
}

impl Mat4x4 {
	pub fn to_bytes(&self) -> &[u8] {
		unsafe { std::slice::from_raw_parts(self.elements.as_ptr() as *const u8, 4 * 16) }
	}
}
