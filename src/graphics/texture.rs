pub struct RawTexture<'a> {
	width: usize,
	height: usize,
	pixels: &'a [[u8; 4]],
}
impl<'a> RawTexture<'a> {
	pub const DEBUG_TEXTURE: Self = Self {
		width: 2,
		height: 2,
		pixels: &[
			[255, 255, 255, 255],
			[0, 255, 0, 255],
			[0, 255, 255, 255],
			[255, 0, 255, 255],
		],
	};
	pub fn size(&self) -> wgpu::Extent3d {
		wgpu::Extent3d {
			width: self.width as u32,
			height: self.height as u32,
			depth_or_array_layers: 1,
		}
	}
	pub fn data_layout(&self) -> wgpu::ImageDataLayout {
		wgpu::ImageDataLayout {
			offset: 0,
			bytes_per_row: Some(4 * self.width as u32),
			rows_per_image: Some(self.height as u32),
		}
	}
	pub fn data(&self) -> &[u8] {
		self.pixels.flatten()
	}
}
impl<'a> Default for RawTexture<'a> {
	fn default() -> Self {
		Self::DEBUG_TEXTURE
	}
}
