file(GLOB SOURCES *.c)
idf_component_register(SRCS "main.cpp" "ES8388.cpp" "TransVoix.cpp" "AudioDsp.cpp"
			INCLUDE_DIRS "")
