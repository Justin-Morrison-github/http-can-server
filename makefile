server:
	gcc ./src/http/server.c ./src/http/http.c -o ./build/http/server &&  ./build/http/server

client:
	gcc ./src/http/client.c -o ./build/http/client &&  ./build/http/client

send_can:
	gcc ./src/can/send_can.c ./src/can/utils.c -o ./build/can/send_can && ./build/can/send_can

read_can:
	gcc ./src/can/read_can.c ./src/can/utils.c -o ./build/can/read_can && ./build/can/read_can

codec:
	gcc ./src/can/codec/codec.c -o ./build/can/codec/codec && ./build/can/codec/codec