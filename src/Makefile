# 定义编译器
CXX=g++
CXXFLAGS=
# 定义源文件和目标文件
CLIENT_SRC=client.cpp
SERVER_SRC=tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp
CLIENT_OBJ=$(CLIENT_SRC:.cpp=.o)
SERVER_OBJ=$(SERVER_SRC:.cpp=.o)
BUILD_DIR=../build
Debug_DIR=$(BUILD_DIR)/Debug
CLIENT_EXE=$(BUILD_DIR)/client
SERVER_EXE=$(BUILD_DIR)/server
Debug_EXE=$(Debug_DIR)/server


# 默认目标
all: $(CLIENT_EXE) $(SERVER_EXE)


# 如何从每个源文件生成对应的 .o 文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 如何从 client.o 生成最终的 client 可执行文件
$(CLIENT_EXE): $(CLIENT_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CLIENT_OBJ) -o $@

# 如何从 server.o 生成最终的 server 可执行文件
$(SERVER_EXE): $(SERVER_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(SERVER_OBJ) -o $@

# 生成gdb可调式文件
debug: CXXFLAGS += -g
debug: $(SERVER_OBJ)
	mkdir -p $(Debug_DIR)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJ) -o $(Debug_EXE)

# 清理生成的文件
clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_EXE) $(SERVER_EXE)