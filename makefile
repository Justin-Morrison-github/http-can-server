server:
	gcc ./src/http/server.c -o ./build/http/server &&  ./build/http/server

client:
	gcc ./src/http/client.c -o ./build/http/client &&  ./build/http/client