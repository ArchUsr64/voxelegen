#![feature(slice_flatten)]
use winit::{
	event::*,
	event_loop::EventLoop,
	keyboard::{KeyCode, PhysicalKey},
	window::WindowBuilder,
};

mod graphics;

#[tokio::main]
async fn main() {
	env_logger::init();
	let event_loop = EventLoop::new().unwrap();
	let window = WindowBuilder::new().build(&event_loop).unwrap();
	window.request_redraw();

	let mut pos = graphics::math::Vec3 {
		x: 0.,
		y: 0.,
		z: 0.,
	};
	let mut state = graphics::Wgpu::new(window).await;
	let _ = event_loop.run(move |event, control_flow| match event {
		Event::WindowEvent {
			ref event,
			window_id,
		} if window_id == state.window().id() => match event {
			WindowEvent::CloseRequested => control_flow.exit(),
			WindowEvent::Resized(new_size) => state.resize(*new_size),
			WindowEvent::RedrawRequested => println!("Redraw"),
			WindowEvent::KeyboardInput { event: x, .. } => match x {
				KeyEvent {
					physical_key: y, ..
				} => match y {
					PhysicalKey::Code(KeyCode::KeyW) => pos.y += 0.05,
					PhysicalKey::Code(KeyCode::KeyS) => pos.y -= 0.05,
					PhysicalKey::Code(KeyCode::KeyA) => pos.x -= 0.05,
					PhysicalKey::Code(KeyCode::KeyD) => pos.x += 0.05,
					PhysicalKey::Code(KeyCode::ShiftLeft) => pos.z -= 0.05,
					PhysicalKey::Code(KeyCode::Space) => pos.z += 0.05,
					_ => {}
				},
			},
			_ => {}
		},
		Event::AboutToWait => {
			state.translation_matrix.elements[3][0] = pos.x;
			state.translation_matrix.elements[3][1] = pos.y;
			state.translation_matrix.elements[3][2] = pos.z;
			state.render().unwrap()
		}
		x => println!("{x:?}"),
	});
}
