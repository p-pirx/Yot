add_executable(
  yacht
  yacht.cpp
)

add_resource(yacht fastnet.tga)

# Pull in pico libraries that we need
target_link_libraries(yacht pico_stdlib pico_display )

# create map/bin/hex file etc.
pico_add_extra_outputs(yacht)


