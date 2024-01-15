all: frontend backend clean

frontend: frontend.c
	gcc frontend.c -o frontend
	@echo "frontend compiled"

backend: backend.c
	gcc backend.c users_lib.o -o backend -pthread
	@echo "backend compiled"

clean:
	rm -f FRONTEND_FIFO_*[0-9][0-9][0-9][0-9]
	rm -f NOTIF_CLI_*[0-9][0-9][0-9][0-9]
	rm BACKENDFIFO
