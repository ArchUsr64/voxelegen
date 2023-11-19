use winit::window::Window;
use winit::{event::*, event_loop::EventLoop, window::WindowBuilder};

struct State {
	surface: wgpu::Surface,
	device: wgpu::Device,
	queue: wgpu::Queue,
	config: wgpu::SurfaceConfiguration,
	size: winit::dpi::PhysicalSize<u32>,
	window: Window,
}

impl State {
	async fn new(window: Window) -> Self {
		let size = window.inner_size();
		let instance = wgpu::Instance::new(wgpu::InstanceDescriptor {
			backends: wgpu::Backends::all(),
			flags: wgpu::InstanceFlags::debugging(),
			dx12_shader_compiler: wgpu::Dx12Compiler::default(),
			gles_minor_version: wgpu::Gles3MinorVersion::Automatic,
		});
		let surface = unsafe { instance.create_surface(&window).unwrap() };
		let adapter = instance
			.request_adapter(&wgpu::RequestAdapterOptionsBase {
				power_preference: wgpu::PowerPreference::None,
				force_fallback_adapter: false,
				compatible_surface: Some(&surface),
			})
			.await
			.unwrap();
		println!("{:?}", adapter.get_info());
		let (device, queue) = adapter
			.request_device(
				&wgpu::DeviceDescriptor {
					label: None,
					features: wgpu::Features::empty(),
					limits: wgpu::Limits::default(),
				},
				None,
			)
			.await
			.unwrap();
		let surface_capabilities = surface.get_capabilities(&adapter);
		let surface_format = surface_capabilities
			.formats
			.iter()
			.copied()
			.find(|f| f.is_srgb())
			.unwrap_or(surface_capabilities.formats[0]);
		let config = wgpu::SurfaceConfiguration {
			usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
			format: surface_format,
			width: size.width,
			height: size.height,
			present_mode: surface_capabilities.present_modes[0],
			alpha_mode: surface_capabilities.alpha_modes[0],
			view_formats: vec![],
		};
		Self {
			surface,
			device,
			queue,
			config,
			size,
			window,
		}
	}

	pub fn window(&self) -> &Window {
		&self.window
	}

	fn resize(&mut self, new_size: winit::dpi::PhysicalSize<u32>) {
		if new_size.width > 0 && new_size.height > 0 {
			self.size = new_size;
			self.config.width = new_size.width;
			self.config.height = new_size.height;
			self.surface.configure(&self.device, &self.config)
		}
	}

	fn render(&mut self) -> Result<(), wgpu::SurfaceError> {
		let output = self.surface.get_current_texture()?;
		let view = output
			.texture
			.create_view(&wgpu::TextureViewDescriptor::default());
		let mut encoder = self
			.device
			.create_command_encoder(&wgpu::CommandEncoderDescriptor { label: None });
		let _render_pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
			label: None,
			color_attachments: &[Some(wgpu::RenderPassColorAttachment {
				view: &view,
				resolve_target: None,
				ops: wgpu::Operations {
					load: wgpu::LoadOp::Clear(wgpu::Color::RED),
					store: wgpu::StoreOp::Store,
				},
			})],
			depth_stencil_attachment: None,
			timestamp_writes: None,
			occlusion_query_set: None,
		});
		drop(_render_pass);
		self.queue.submit(std::iter::once(encoder.finish()));
		output.present();
		Ok(())
	}
}

#[tokio::main]
async fn main() {
	env_logger::init();
	let event_loop = EventLoop::new().unwrap();
	let window = WindowBuilder::new().build(&event_loop).unwrap();

	let mut state = State::new(window).await;
	let _ = event_loop.run(move |event, control_flow| match event {
		Event::WindowEvent {
			ref event,
			window_id,
		} if window_id == state.window().id() => match event {
			WindowEvent::CloseRequested => control_flow.exit(),
			WindowEvent::Resized(new_size) => state.resize(*new_size),
			_ => {}
		},
		Event::AboutToWait => state.render().unwrap(),
		_ => (),
	});
}
