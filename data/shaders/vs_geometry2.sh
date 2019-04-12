
    //mat4 normalModelView = transpose(inverse(modelView));
    mat4 normalModelView = modelView;

    int material_index = int(u_state_material_vertex);
    BaseMaterial basic = read_base_material(material_index);

    v_color = a_color0;
    v_uv0 = (a_texcoord0 * basic.uv0_scale) + basic.uv0_offset;
    v_uv1 = (a_texcoord1 * basic.uv1_scale) + basic.uv1_offset;

    v_view = mul(modelView, vec4(a_position.xyz, 1.0)).xyz;
    
    if(u_qnormals)
        v_normal = normalize(mul(normalModelView, vec4(vec3(a_normal) / 255.0 - 0.5, 0.0)).xyz);
    else
        v_normal = normalize(mul(normalModelView, vec4(a_normal, 0.0)).xyz);

    v_position = mul(u_proj, vec4(v_view, 1.0));

    if(u_vflip)
        v_position.y = -v_position.y;
