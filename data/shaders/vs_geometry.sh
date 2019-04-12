
    //mat4 normalModelView = transpose(inverse(modelView));
    mat4 normalModelView = modelView;

    int material_index = int(u_state_material_vertex);
    BaseMaterial basic = read_base_material(material_index);

    v_color = a_color0;
    v_uv0 = (a_texcoord0 * basic.uv0_scale) + basic.uv0_offset;
    v_uv1 = (a_texcoord1 * basic.uv1_scale) + basic.uv1_offset;
    
    vec3 position = a_position.xyz;
    
#ifdef MORPHTARGET
    position += a_morph0 * u_morph_weights[0];
    position += a_morph1 * u_morph_weights[1];
    position += a_morph2 * u_morph_weights[2];
    position += a_morph3 * u_morph_weights[3];
#endif

    if(u_displace)
    {
        LitMaterial matlit = read_lit_material(material_index);
        float displace = texture2D(s_displace, v_uv0).x;
        position += normalize(a_normal) * (displace * matlit.displace + matlit.displace_bias);
    }

    v_view = mul(modelView, vec4(position, 1.0)).xyz;
    
    if(u_qnormals)
    {
        v_normal = normalize(mul(normalModelView, vec4(a_normal / 255.0 - 0.5, 0.0)).xyz);
        v_tangent = normalize(mul(normalModelView, vec4(a_tangent.xyz / 255.0 - 0.5, 0.0)).xyz);
    }
    else
    {
        v_normal = normalize(mul(normalModelView, vec4(a_normal, 0.0)).xyz);
        v_tangent = normalize(mul(normalModelView, vec4(a_tangent.xyz, 0.0)).xyz);
    }
    
    vec3 binormal = normalize(a_tangent.a * cross(a_normal, a_tangent.xyz));
    v_binormal = normalize(mul(normalModelView, vec4(binormal, 0.0)).xyz);

    v_position = mul(u_proj, vec4(v_view, 1.0));

