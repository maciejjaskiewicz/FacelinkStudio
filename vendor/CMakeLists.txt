set(XENON_BUILD_EXAMPLES OFF CACHE BOOL "Build Xenon examples" FORCE)
set(XENON_BUILD_TESTS OFF CACHE BOOL "Build Xenon tests" FORCE)

add_subdirectory(Xenon)
set_target_properties(Xenon PROPERTIES FOLDER Vendor)