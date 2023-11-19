struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) color: vec3<f32>,
};

@vertex
fn vs_main(
    @builtin(vertex_index) in_vertex_index: u32
) -> VertexOutput {
    var out: VertexOutput;
    let x = f32(1 - i32(in_vertex_index)) * 0.5;
    let y = f32(i32(in_vertex_index & 1u) * 2 - 1) * 0.5;
    out.color = vec3<f32>(0.0, 0.0, 0.0);
    if i32(in_vertex_index) == 0 {
        out.color.r = 1.0;
    }
    if i32(in_vertex_index) == 1 {
        out.color.g = 1.0;
    }
    if i32(in_vertex_index) == 2 {
        out.color.b = 1.0;
    }
    out.clip_position = vec4<f32>(x, y, 0.0, 1.0);
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    return vec4<f32>(in.color.r, in.color.g, in.color.b, 1.0);
}
