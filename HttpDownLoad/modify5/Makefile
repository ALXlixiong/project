all:httpmain
httpmain:httpmain.cc client.hpp 
	g++ $^ -g -o $@ -std=c++11 -fpermissive -lpthread
.PHONY:clean
clean:
	rm httpmain *.*td seek_file.txt ./id/* *.doc
