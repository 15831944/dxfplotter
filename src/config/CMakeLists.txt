set_property(SOURCE config.h PROPERTY SKIP_AUTOGEN ON)
set(SRC
	config.cpp
	item.cpp

	config.h
	item.h
	variable.h
)

add_library(config ${SRC})
add_dependencies(config generate_config)

add_custom_command(
	OUTPUT config.h
	COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/config_gen.py ${TEMPLATE_DIR}/config.xml ${CMAKE_CURRENT_SOURCE_DIR} config.h.in config.h
	DEPENDS config.h.in ${TEMPLATE_DIR}/config.xml
)

add_custom_target(generate_config DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config.h)
