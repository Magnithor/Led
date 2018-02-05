# main compiler

CC=/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/bin/mipsel-openwrt-linux-gcc
CXX=/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/bin/mipsel-openwrt-linux-g++
LD=/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/bin/mipsel-openwrt-linux-ld
CFLAGS_ORG=-Os -std=c++11 -pipe -mno-branch-likely -mips32r2 -mtune=24kc -fno-caller-saves -fno-plt -fhonour-copts -Wno-error=unused-but-set-variable -Wno-error=unused-result -msoft-float -mips16 -minterlink-mips16 -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro -I /home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/usr/include -I /home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/include -I /home/magni/source/staging_dir/target-mipsel_24kc_musl/usr/include -I /home/magni/source/staging_dir/target-mipsel_24kc_musl/include
CFLAGS=-lstdc++ -Werror -Wall -g -Os -std=c++11 -pipe -mno-branch-likely -mips32r2 -mtune=24kc -fno-caller-saves -fno-plt -fhonour-copts -Wno-error=unused-but-set-variable -Wno-error=unused-result -msoft-float -mips16 -minterlink-mips16 -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro -I /home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/usr/include -I /home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/include -I /home/magni/source/staging_dir/target-mipsel_24kc_musl/usr/include -I /home/magni/source/staging_dir/target-mipsel_24kc_musl/include
LDFLAGS=-L/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/usr/lib -L/home/magni/source/staging_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/lib -L/home/magni/source/staging_dir/target-mipsel_24kc_musl/usr/lib -L/home/magni/source/staging_dir/target-mipsel_24kc_musl/lib
USER_LIBS=
TEST_CFLAGS=-lstdc++ -Werror -Wall -g -Os -std=c++11
#KERNEL = /home/magni/source/build_dir/toolchain-mipsel_24kc_gcc-5.5.0_musl/linux-4.9.67

TEST_OBJS = test.o jsonTest.o testJson.o
OBJS = json.o apa102.o playBackItem.o httpServer.o urls.o module.o fastgpio.o fastgpioomega2.o playBack.o main.o playBackItemSolid.o playBackItemSlide.o playBackItemFade.o mqtt.o

export STAGING_DIR="/home/magni/source/staging_dir"

TARGET1 := main
OUTFILE := led

all: test led

led: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(OUTFILE) $(LDFLAGS)

mqtt.o: mqtt.cpp
	$(CXX) $(CFLAGS) -c mqtt.cpp  $(LDFLAGS)

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

playBackItemSlide.o: playBackItemSlide.cpp
	$(CXX) $(CFLAGS) -c playBackItemSlide.cpp  $(LDFLAGS)

playBackItemFade.o: playBackItemFade.cpp
	$(CXX) $(CFLAGS) -c playBackItemFade.cpp  $(LDFLAGS)

urls.o: urls.cpp
	$(CXX) $(CFLAGS) -c urls.cpp  $(LDFLAGS)

json.o: json.cpp
	$(CXX) $(CFLAGS) -c json.cpp  $(LDFLAGS)

jsonTest.o: json.cpp
	g++ $(TEST_CFLAGS) -c json.cpp -o jsonTest.o

testJson.o: testJson.cpp
	g++ $(TEST_CFLAGS) -c testJson.cpp 

test.o: test.cpp
	g++ $(TEST_CFLAGS) -c test.cpp 

test: clean $(TEST_OBJS) 
	g++ $(TEST_CFLAGS) $(TEST_OBJS) -o test -lcppunit 
	./test

clean:
	@rm -rf *.o $(OUTFILE)
	echo "clean done"
