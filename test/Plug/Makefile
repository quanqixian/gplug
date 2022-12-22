TARGET=test
$(TARGET): main.cpp
	g++ $^ -o $@            \
        -I../../include/    \
        -I../../include/interface    \
        -L../../lib/        \
        -Wl,-rpath='$$ORIGIN:$$ORIGIN/../../lib' \
        -lgplug \
        -lpthread\
		-ldl

.PHONY:clean
clean:
	rm -rf $(TARGET)
