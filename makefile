# main compiler

CC=/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/bin/mipsel-openwrt-linux-gcc
CXX=/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/bin/mipsel-openwrt-linux-g++
LD=/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/bin/mipsel-openwrt-linux-ld
CFLAGS=-Os -std=c++11 -pipe -mno-branch-likely -mips32r2 -mtune=24kc -fno-caller-saves -fno-plt -fhonour-copts -Wno-error=unused-but-set-variable -Wno-error=unused-result -msoft-float -mips16 -minterlink-mips16 -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro -I /home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/usr/include -I /home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/include -I /home/magni/source/staging_dir/target-mipsel_24kc_musl/usr/include -I /home/magni/source/staging_dir/target-mipsel_24kc_musl/include
LDFLAGS=-L/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/usr/lib -L/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/lib -L/home/magni/source/staging_dir/target-mipsel_24kc_musl/usr/lib -L/home/magni/source/staging_dir/target-mipsel_24kc_musl/lib
USER_LIBS=
#KERNEL = /home/magni/source/build_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/linux-4.9.67

export STAGING_DIR="/home/magni/source/staging_dir"

TARGET1 := main
OUTFILE := led

all: led

led: apa102.o playBackItem.o httpServer.o urls.o module.o fastgpio.o fastgpioomega2.o playBack.o main.o playBackItemSolid.o 
	$(CXX) $(CFLAGS) apa102.o playBackItem.o playBack.o httpServer.o urls.o module.o fastgpio.o fastgpioomega2.o playBackItemSolid.o main.o -o $(OUTFILE) $(LDFLAGS)

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp  $(LDFLAGS)

module.o: module.cpp
	$(CXX) $(CFLAGS) -c module.cpp  $(LDFLAGS)

fastgpioomega2.o: fastgpioomega2.cpp
	$(CXX) $(CFLAGS) -c fastgpioomega2.cpp  $(LDFLAGS)

fastgpio.o: fastgpio.cpp
	$(CXX) $(CFLAGS) -c fastgpio.cpp  $(LDFLAGS)

apa102.o: apa102.cpp
	$(CXX) $(CFLAGS) -c apa102.cpp  $(LDFLAGS)

httpServer.o: httpServer.cpp
	$(CXX) $(CFLAGS) -c httpServer.cpp  $(LDFLAGS)

playBack.o: playBack.cpp
	$(CXX) $(CFLAGS) -c playBack.cpp  $(LDFLAGS)

playBackItem.o: playBackItem.cpp
	$(CXX) $(CFLAGS) -c playBackItem.cpp  $(LDFLAGS)

playBackItemSolid.o: playBackItemSolid.cpp
	$(CXX) $(CFLAGS) -c playBackItemSolid.cpp  $(LDFLAGS)

urls.o: urls.cpp
	$(CXX) $(CFLAGS) -c urls.cpp  $(LDFLAGS)

clean:
	@rm -rf *.o $(OUTFILE)
