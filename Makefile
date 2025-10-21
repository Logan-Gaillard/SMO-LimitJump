.PHONY: all clean update-headers

all: configure
	cmake --build build -j$(shell nproc)
	$(MAKE) copytosuyu

configure:
	cmake --toolchain=cmake/toolchain.cmake -S . -B build

clean:
	-rm -r build

update-headers:
	git submodule update --remote ./include/

copytosuyu:
	mkdir -p /mnt/c/Users/Logan/AppData/Roaming/suyu/load/0100000000010000/LimitJump/exefs
	cp ./build/main.npdm ./build/subsdk9 /mnt/c/Users/Logan/AppData/Roaming/suyu/load/0100000000010000/LimitJump/exefs/

romfstosuyu:
	mkdir -p /mnt/c/Users/Logan/AppData/Roaming/suyu/load/0100000000010000/LimitJump/romfs
	cp -r ./romfs/* /mnt/c/Users/Logan/AppData/Roaming/suyu/load/0100000000010000/LimitJump/romfs/