# dfrog_set_warnings(<target>)
# Attaches a strict, portable warning set to <target>. WError is opt-in via
# the DFROG_WERROR option (defaults to ON in dev/release presets).
function(dfrog_set_warnings target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "dfrog_set_warnings: '${target}' is not a target")
    endif()

    set(_gcc_clang_flags
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
    )

    set(_msvc_flags
        /W4
        /permissive-
        /w14242 /w14254 /w14263 /w14265 /w14287 /w14296
        /w14311 /w14545 /w14546 /w14547 /w14549 /w14555
        /w14619 /w14640 /w14826 /w14905 /w14906 /w14928
    )

    if(MSVC)
        target_compile_options(${target} PRIVATE ${_msvc_flags})
        if(DFROG_WERROR)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE ${_gcc_clang_flags})
        if(DFROG_WERROR)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()
