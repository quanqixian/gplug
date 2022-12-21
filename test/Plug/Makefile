TARGET=test
$(TARGET): main.cpp
	g++ $^ -o $@            \
        -I../../include/    \
        -L../../lib/        \
        -Wl,-rpath='$$ORIGIN:$$ORIGIN/../../lib' \
        -lgplug \
        -lpthread\
		-ldl

.PHONY:clean
clean:
	rm -rf $(TARGET)
