# Makefile for compiling and linking C++ files  

# 编译器  
CXX = g++  
  
# 编译选项  
CXXFLAGS = -std=c++17 -O0
  
# 目标文件  
OBJS = lexical_analysis.o my_dfa.o lex.o  
  
# 可执行文件  
TARGET = lex
  
# 默认目标  
all: $(TARGET)  
  
# 编译目标文件  
%.o: %.cpp  
	$(CXX) $(CXXFLAGS) -c $< -o $@  
  
# 链接可执行文件  
$(TARGET): $(OBJS)  
	$(CXX) $(OBJS) -o $(TARGET)  
  
# 清理生成的文件  
clean:  
	rm -f $(OBJS) $(TARGET)  
  
# 伪目标，不是实际文件  
.PHONY: all clean