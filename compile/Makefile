all:oj_server compile_server
oj_server:oj_server.cc oj_model.hpp oj_view.hpp
	g++ $^ -g -o $@ -std=c++11 \
			-lpthread -ljsoncpp -lctemplate \
			-I ~/third_part/include -L ~/third_part/lib
compile_server:compile_server.cc CompileRun.hpp
	g++ $^ -g -o $@ -std=c++11 -lpthread -ljsoncpp
.PHONY:clean
clean:
	rm compile_server oj_server
