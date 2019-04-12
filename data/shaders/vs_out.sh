
#if defined RENDER_UV
    vec2 uv2ndc = mod(vec2(v_uv0.x, 1.0 - v_uv0.y), vec2(1.0, 1.0)) * 2.0 - 1.0; // uvs are relative to top-left, so we need to reverse y and to NDC for rendering
    gl_Position = vec4(uv2ndc, 0.0, 1.0);
#elif defined RENDER_UV2
    vec2 uv2ndc = mod(vec2(v_uv1.x, 1.0 - v_uv1.y), vec2(1.0, 1.0)) * 2.0 - 1.0; // uvs are relative to top-left, so we need to reverse y and to NDC for rendering
    gl_Position = vec4(uv2ndc, 0.0, 1.0);
#else
    gl_Position = v_position;
#endif

    if(u_vflip)
    {
        gl_Position.y = -gl_Position.y;
        v_position.y = -v_position.y;
    }
