TARGET=test
$(TARGET): main.cpp
	g++ $^ -o $@          \
        -I../include/    \
        -L../bin/         \
        -Wl,-rpath='$$ORIGIN:$$ORIGIN/../bin' \
        -lgplug \
        -lpthread\
		-ldl

.PHONY:clean
clean:
	rm -rf $(TARGET)
