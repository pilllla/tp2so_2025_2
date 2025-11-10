CC = gcc

#-Wall (ativa todos os warnings)
#-g (inclui símbolos de debug)
#-Iinclude (informa ao compilador para procurar headers na pasta 'include')
CFLAGS = -Wall -g -Iinclude

#-lm (matemática)
LDFLAGS = -lm

SRCS = src/simulador.c

TARGET = simulador

#'make'
.PHONY: all
all: $(TARGET)

#regra
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

#teste simples (com 4 quadros e modo debug)
.PHONY: run_test1
run_test1: all
#	@echo "--- Executando teste_preenchimento (LRU, 4KB pag, 16KB mem, debug=1) ---"
	@./$(TARGET) lru our_tests/todos_4_16_1_preenchimento.log 4 16 1

.PHONY: run_test2
run_test2: all
	@./$(TARGET) lfu our_tests/todos_4_16_1_suja.log 4 16 1

.PHONY: run_test3_1
run_test3_1: all
	@./$(TARGET) random our_tests/todos_4_16_1_subs.log 4 16 1

.PHONY: run_test3_2
run_test3_2: all
	@./$(TARGET) lfu our_tests/todos_4_16_1_subs.log 4 16 1

.PHONY: run_test4_1
run_test4_1: all
	@./$(TARGET) lru our_tests/lru_lfu.log 4 16 1
 
.PHONY: run_test4_2
run_test4_2: all
	@./$(TARGET) lfu our_tests/lru_lfu.log 4 16 1

.PHONY: clean
clean:
	rm -f $(TARGET)