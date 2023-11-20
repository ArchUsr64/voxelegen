use wgpu::util::DeviceExt;
use winit::window::Window;
use winit::{event::*, event_loop::EventLoop, window::WindowBuilder};

#[repr(C)]
#[derive(Clone, Copy, Debug)]
struct Vertex {
	position: [f32; 3],
	color: [f32; 3],
}
impl Vertex {
	const ATTRIBS: [wgpu::VertexAttribute; 2] =
		wgpu::vertex_attr_array![0 => Float32x3, 1 => Float32x3];

	const fn new(x: f32, y: f32, z: f32, color: wgpu::Color) -> Vertex {
		Self {
			position: [x, y, z],
			color: [color.r as f32, color.g as f32, color.b as f32],
		}
	}
	const fn desc() -> wgpu::VertexBufferLayout<'static> {
		wgpu::VertexBufferLayout {
			array_stride: std::mem::size_of::<Vertex>() as u64,
			step_mode: wgpu::VertexStepMode::Vertex,
			attributes: &Self::ATTRIBS,
		}
	}
}

const VERT_BUF: &[Vertex] = &[
	Vertex::new(1., 1., 0., wgpu::Color::RED),
	Vertex::new(-1., 1., 0., wgpu::Color::GREEN),
	Vertex::new(-1., -1., 0., wgpu::Color::BLUE),
	Vertex::new(1., -1., 0., wgpu::Color::BLACK),
];

const INDEX_BUF: &[u16] = &[2, 0, 1, 2, 3, 0];

struct State<'a> {
	surface: wgpu::Surface,
	device: wgpu::Device,
	queue: wgpu::Queue,
	config: wgpu::SurfaceConfiguration,
	size: winit::dpi::PhysicalSize<u32>,
	window: &'a Window,
	pipeline: wgpu::RenderPipeline,
	vertex_buffer: wgpu::Buffer,
	index_buffer: wgpu::Buffer,
}

impl<'a> State<'a> {
	async fn new(window: &'a Window) -> State<'a> {
		let size = window.inner_size();
		let instance = wgpu::Instance::new(wgpu::InstanceDescriptor {
			backends: wgpu::Backends::all(),
			flags: wgpu::InstanceFlags::all(),
			dx12_shader_compiler: wgpu::Dx12Compiler::default(),
			gles_minor_version: wgpu::Gles3MinorVersion::Automatic,
		});
		let surface = unsafe { instance.create_surface(window).unwrap() };
		let adapter = instance
			.request_adapter(&wgpu::RequestAdapterOptionsBase {
				power_preference: wgpu::PowerPreference::None,
				force_fallback_adapter: false,
				compatible_surface: Some(&surface),
			})
			.await
			.unwrap();
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
		let shader = device.create_shader_module(wgpu::include_wgsl!("shader.wgsl"));
		let pipeline_layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
			label: None,
			bind_group_layouts: &[],
			push_constant_ranges: &[],
		});
		let pipeline = device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
			label: None,
			layout: Some(&pipeline_layout),
			vertex: wgpu::VertexState {
				module: &shader,
				entry_point: "vs_main",
				buffers: &[Vertex::desc()],
			},
			fragment: Some(wgpu::FragmentState {
				module: &shader,
				entry_point: "fs_main",
				targets: &[Some(wgpu::ColorTargetState {
					format: config.format,
					blend: Some(wgpu::BlendState::REPLACE),
					write_mask: wgpu::ColorWrites::ALL,
				})],
			}),
			primitive: wgpu::PrimitiveState {
				topology: wgpu::PrimitiveTopology::TriangleList,
				strip_index_format: None,
				front_face: wgpu::FrontFace::Ccw,
				cull_mode: Some(wgpu::Face::Back),
				unclipped_depth: false,
				polygon_mode: wgpu::PolygonMode::Fill,
				conservative: false,
			},
			depth_stencil: None,
			multisample: wgpu::MultisampleState {
				count: 1,
				mask: !0,
				alpha_to_coverage_enabled: false,
			},
			multiview: None,
		});
		let vertex_buffer = unsafe {
			device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
				contents: {
					std::slice::from_raw_parts(
						VERT_BUF.as_ptr() as *const u8,
						std::mem::size_of_val(VERT_BUF),
					)
				},
				label: None,
				usage: wgpu::BufferUsages::VERTEX,
			})
		};
		let index_buffer = unsafe {
			device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
				contents: {
					std::slice::from_raw_parts(
						INDEX_BUF.as_ptr() as *const u8,
						std::mem::size_of_val(INDEX_BUF),
					)
				},
				label: None,
				usage: wgpu::BufferUsages::INDEX,
			})
		};
		Self {
			surface,
			device,
			queue,
			config,
			size,
			window,
			pipeline,
			vertex_buffer,
			index_buffer,
		}
	}

	pub fn window(&self) -> &Window {
		self.window
	}

	fn resize(&mut self, new_size: winit::dpi::PhysicalSize<u32>) {
		self.size = new_size;
		self.config.width = new_size.width;
		self.config.height = new_size.height;
		self.surface.configure(&self.device, &self.config)
	}

	fn render(&mut self) -> Result<(), wgpu::SurfaceError> {
		let output = self.surface.get_current_texture()?;
		let view = output
			.texture
			.create_view(&wgpu::TextureViewDescriptor::default());
		let mut encoder = self
			.device
			.create_command_encoder(&wgpu::CommandEncoderDescriptor { label: None });
		let mut render_pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
			label: None,
			color_attachments: &[Some(wgpu::RenderPassColorAttachment {
				view: &view,
				resolve_target: None,
				ops: wgpu::Operations {
					load: wgpu::LoadOp::Clear(wgpu::Color::BLACK),
					store: wgpu::StoreOp::Store,
				},
			})],
			depth_stencil_attachment: None,
			timestamp_writes: None,
			occlusion_query_set: None,
		});
		render_pass.set_pipeline(&self.pipeline);
		render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
		render_pass.set_index_buffer(self.index_buffer.slice(..), wgpu::IndexFormat::Uint16);
		render_pass.draw_indexed(0..(INDEX_BUF.len() as u32), 0, 0..1);
		drop(render_pass);
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

	let mut state = State::new(&window).await;
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
