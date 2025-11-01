# --------------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------------

find_package(yat CONFIG REQUIRED)
find_package(ufxclib CONFIG REQUIRED)

# --------------------------------------------------------------------------
# Collect sources and includes
# --------------------------------------------------------------------------

file(GLOB_RECURSE sources src/*.cpp)
set(includedirs
    include/
)

# --------------------------------------------------------------------------
# Target
# --------------------------------------------------------------------------

add_library(limaufxc ${sources})
target_include_directories(limaufxc 
    PUBLIC 
        ${includedirs}
)
target_link_libraries(limaufxc 
    PUBLIC 
        limacore
        yat::yat
        ufxclib::ufxclib
)

limatools_set_library_soversion(limaufxc "VERSION")

# --------------------------------------------------------------------------
# Compile definitions
# --------------------------------------------------------------------------

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" UFXC_VERSION_RAW)
string(STRIP "${UFXC_VERSION_RAW}" UFXC_VERSION)

target_compile_definitions(limaufxc
    PUBLIC
        UFXC_NAME=Ufxc
        UFXC_VERSION=${UFXC_VERSION}
        UFXC_ENABLED
)

message(STATUS "Camera enabled: Ufxc ${UFXC_VERSION}")

# --------------------------------------------------------------------------
# Install
# --------------------------------------------------------------------------

install(
    TARGETS limaufxc 
    LIBRARY DESTINATION lib
)
