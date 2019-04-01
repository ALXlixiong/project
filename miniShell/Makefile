.PHONY:all test 
test:test.c
	gcc $^ -g -o $@

main:myshell.c
	gcc $^ -g -Wall -o $@

.PHONY:clean
clean:
	rm main test
