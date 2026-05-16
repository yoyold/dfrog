# dfrog_apply_sanitizers(<target>)
# Applies AddressSanitizer+UBSan or ThreadSanitizer based on the
# DFROG_ENABLE_ASAN / DFROG_ENABLE_TSAN cache options. These are mutually
# exclusive — ASan and TSan cannot be combined.
function(dfrog_apply_sanitizers target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "dfrog_apply_sanitizers: '${target}' is not a target")
    endif()

    if(DFROG_ENABLE_ASAN AND DFROG_ENABLE_TSAN)
        message(FATAL_ERROR
            "DFROG_ENABLE_ASAN and DFROG_ENABLE_TSAN cannot both be enabled. "
            "ASan and TSan instrumentations are incompatible.")
    endif()

    if(MSVC)
        if(DFROG_ENABLE_TSAN)
            message(FATAL_ERROR "ThreadSanitizer is not supported on MSVC. "
                "Use Clang/GCC for TSan builds.")
        endif()
        if(DFROG_ENABLE_ASAN)
            target_compile_options(${target} PRIVATE /fsanitize=address)
        endif()
        return()
    endif()

    if(DFROG_ENABLE_ASAN)
        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
            -fno-sanitize-recover=all
        )
        target_link_options(${target} PRIVATE
            -fsanitize=address,undefined
        )
    elseif(DFROG_ENABLE_TSAN)
        target_compile_options(${target} PRIVATE
            -fsanitize=thread
            -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE
            -fsanitize=thread
        )
    endif()
endfunction()
