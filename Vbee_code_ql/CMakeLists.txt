# Copyright (C) 2020 QUECTEL Technologies Limited and/or its affiliates("QUECTEL").
# All rights reserved.
#

set(target Vbee_code_ql)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_app_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_QUEC)
target_include_directories(${target} PUBLIC ./
							../components/ql-application/LinkSDK/components/subdev
							./mqtt
							./uart
							)

#target_link_libraries(${target} PRIVATE kernel driver ql_api_common)



target_sources(${target} PRIVATE
		../components/ql-application/LinkSDK/components/subdev/cJSON.c 
		bee_main.c 
		./mqtt/bee_mqtt.c
		./mqtt/bee_mqtt_tx.c 
		./mqtt/bee_mqtt_rx.c 
		./uart/bee_uart.c

)
relative_glob(srcs include/*.h src/*.c inc/*.h)
beautify_c_code(${target} ${srcs})
