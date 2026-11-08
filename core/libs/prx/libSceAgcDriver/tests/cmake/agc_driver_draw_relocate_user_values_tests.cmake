if(BUILD_TESTING)
    add_test(NAME agc_driver_draw_runtime_uniforms COMMAND agc_driver_draw_relocate_user_values_tests --runtime-uniforms)
    add_test(NAME agc_driver_draw_runtime_uniforms_reference COMMAND agc_driver_draw_relocate_user_values_tests --runtime-reference)
    set_tests_properties(agc_driver_draw_runtime_uniforms agc_driver_draw_runtime_uniforms_reference PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 ENVIRONMENT "ANYPS5_NO_SHADER_CACHE=1")
    set_property(TEST agc_driver_draw_runtime_uniforms_reference APPEND PROPERTY ENVIRONMENT "APS5_NO_DRAW_RELOCATION=1")
    if(WIN32)
        set_tests_properties(agc_driver_draw_runtime_uniforms agc_driver_draw_runtime_uniforms_reference PROPERTIES
                ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:libc>")
    endif()
endif()
