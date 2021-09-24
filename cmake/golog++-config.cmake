include(CMakeFindDependencyMacro)
find_dependency(PkgConfig)
find_dependency(Boost REQUIRED COMPONENTS filesystem program_options)
find_dependency(EclipseClp)
find_dependency(Threads)
pkg_check_modules(SWIPL "swipl" QUIET)
include("${CMAKE_CURRENT_LIST_DIR}/golog++-targets.cmake")

