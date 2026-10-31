# test/Makefile
CC = gcc

CFLAGS = -I../Core/Inc -IUnity -Wall -Wextra -g

UNITY_SRC = Unity/unity.c
UNITY_OBJ = Unity/unity.o
UNITY_HDR = Unity/unity.h Unity/unity_internals.h
LDFLAGS =

CFLAGS += -I$(UNITY_DIR)/src

# Список тестовых файлов
TEST_SRCS = main.c test_parser.c test_format.c # test_reader.c test_estimator.c

# Исходники тестируемых модулей (только не зависящие от HAL)
APP_SRCS = ../Core/Src/format.c ../Core/Src/parser.c

# Исходники Unity
UNITY_SRC = Unity/unity.c

# Если модуль использует HAL, добавьте сюда mock-файл
# MOCK_SRCS = mocks/hal_mock.c

SRCS = $(TEST_SRCS) $(APP_SRCS) $(UNITY_SRC) # $(MOCK_SRCS)
OBJS = $(SRCS:.c=.o)
TARGET = test_runner.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	del /Q *.o $(TARGET) 2>nul