all:oj_server 
oj_server:oj_server.cc oj_model.hpp oj_view.hpp CompileRun.hpp
	g++ $^ -g -o $@ -std=c++11 \
			-lpthread -ljsoncpp -lctemplate \
			-I ~/third_part/include -L ~/third_part/lib \
			`mysql_config --cflags --libs`
compile_server:compile_server.cc CompileRun.hpp
	g++ $^ -g -o $@ -std=c++11 -lpthread -ljsoncpp
.PHONY:clean
clean:
	rm compile_server oj_server
