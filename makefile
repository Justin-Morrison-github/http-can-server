.PHONY: server client send_can read_can codec can_node test_can
server:
	gcc ./src/http/server.c ./src/http/http.c ./src/utils/utils.c -o ./build/http/server &&  ./build/http/server

client:
	gcc ./src/http/client.c ./src/http/http.c -o ./build/http/client &&  ./build/http/client

send_can:
	gcc ./src/can/send_can.c ./src/can/utils.c ./src/can/msg/test_u64/test_u64.c ./src/logger/logger.c -o ./build/can/send_can && ./build/can/send_can

read_can:
	gcc ./src/can/read_can.c ./src/can/utils.c ./src/can/msg/test_u64/test_u64.c ./src/logger/logger.c -o ./build/can/read_can && ./build/can/read_can

codec:
	gcc -g -O0 ./src/can/codec/*.c -lm -o ./build/can/codec/codec && ./build/can/codec/codec

test_can:
	gcc -g ./src/can/test_can.c  ./src/can/utils.c -o ./build/can/test_can && ./build/can/test_can

bridge:
	gcc ./src/bridge/bridge.c ./src/can/utils.c ./src/http/http.c ./src/can/msg/test_u64/test_u64.c ./src/logger/logger.c -o ./build/bridge/bridge && ./build/bridge/bridge

cpu:
	mkdir -p build/nodes/cpu
	gcc src/nodes/cpu/cpu.c src/can/utils.c src/can/msg/cpu_info/cpu_info.c src/logger/logger.c -o build/nodes/cpu/cpu && build/nodes/cpu/cpu

can_logger:
	mkdir -p build/nodes/can_logger
	gcc src/nodes/can_logger/can_logger.c src/can/utils.c src/logger/logger.c -o build/nodes/can_logger/can_logger && build/nodes/can_logger/can_logger
	
clean:
	rm -rf build/*
	rm -rf logs/*