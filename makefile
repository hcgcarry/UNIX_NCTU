all:main

%:%.cpp
	g++ $< -o $@ -g
clean:
	rm main