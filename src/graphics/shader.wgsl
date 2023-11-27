@group(0) @binding(0)
var t_diffuse: texture_2d<f32>;
@group(0)@binding(1)
var s_diffuse: sampler;

@group(1)@binding(0)
var<uniform> val: f32;
@group(1)@binding(1)
var<uniform> matrix: mat4x4<f32>;

struct VertexInput {
    @location(0) pos: vec3<f32>,
    @location(1) uv: vec2<f32>,
}

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) uv: vec2<f32>,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.uv = in.uv;
    var rot_x = mat4x4<f32>(
        vec4<f32>(1., 0., 0., 0.),
        vec4<f32>(0., cos(val), -sin(val), 0.),
        vec4<f32>(0., sin(val), cos(val), 0.),
        vec4<f32>(0., 0., 0., 1.),
    );
    var rot_y = mat4x4<f32>(
        vec4<f32>(cos(val), 0., sin(val), 0.),
        vec4<f32>(0., 1., 0., 0.),
        vec4<f32>(-sin(val), 0., cos(val), 0.),
        vec4<f32>(0., 0., 0., 1.),
    );
    var rot_z = mat4x4<f32>(
        vec4<f32>(cos(val), -sin(val), 0., 0.),
        vec4<f32>(sin(val), cos(val), 0., 0.),
        vec4<f32>(0., 0., 1., 0.),
        vec4<f32>(0., 0., 0., 1.),
    );
    out.clip_position = matrix * rot_z * rot_y * rot_x * vec4<f32>(in.pos, 1.);
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    return textureSample(t_diffuse, s_diffuse, in.uv);
}
