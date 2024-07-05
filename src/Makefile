# 定义编译器
CXX=g++

# 定义源文件和目标文件
CLIENT_SRC=client.cpp
SERVER_SRC=tcpepoll.cpp
CLIENT_OBJ=$(CLIENT_SRC:.cpp=.o)
SERVER_OBJ=$(SERVER_SRC:.cpp=.o)
BULID_DIR=../build
CLIENT_EXE=../build/client
SERVER_EXE=../build/server

# 默认目标
all: $(CLIENT_EXE) $(SERVER_EXE)

# 如何从 client.cpp 生成 client.o
$(CLIENT_OBJ): $(CLIENT_SRC)
	$(CXX) -c $< -o $@

# 如何从 tcpepoll.cpp 生成 server.o
$(SERVER_OBJ): $(SERVER_SRC)
	$(CXX) -c $< -o $@

# 如何从 client.o 生成最终的 client 可执行文件
$(CLIENT_EXE): $(CLIENT_OBJ)
	mkdir -p $(BULID_DIR)
	$(CXX) $(CLIENT_OBJ) -o $@
	rm -f $(CLIENT_OBJ)

# 如何从 server.o 生成最终的 server 可执行文件
$(SERVER_EXE): $(SERVER_OBJ)
	mkdir -p $(BULID_DIR)
	$(CXX) $(SERVER_OBJ) -o $@
	rm -f $(SERVER_OBJ)

# 清理生成的文件
clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_EXE) $(SERVER_EXE)