all: httpd

httpd: httpd.c
	aarch64-linux-gnu-gcc httpd.c -W -Wall -lpthread -o xlinweb 
	# gcc httpd.c -W -Wall -lpthread -o httpd 

clean:
	rm httpd
