main:main.cpp MyDb.cpp
	g++ $^ -g -o $@ `mysql_config --cflags --libs`
.PHONY:clean
clean:
	rm main
