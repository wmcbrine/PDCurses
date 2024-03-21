include(CheckSymbolExists)

check_symbol_exists("vsnprintf" "stdio.h" HAVE_VSNPRINTF)
check_symbol_exists("vsscanf" "stdio.h" HAVE_VSSCANF)

add_compile_definitions(
$<$<BOOL:${HAVE_VSNPRINTF}>:HAVE_VSNPRINTF>
$<$<BOOL:${HAVE_VSSCANF}>:HAVE_VSSCANF>
)

# --- compiler options
add_compile_options(-Wall)
