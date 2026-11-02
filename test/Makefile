# test/Makefile
CC = gcc

# 🔹 Определите корень Unity (папка, куда вы скопировали репозиторий Unity)
UNITY_DIR = ..\..\Unity

# 🔹 Добавьте путь к заголовкам Fixture и базовым заголовкам Unity
CFLAGS = -I../Core/Inc \
         -I$(UNITY_DIR)/src \
         -I$(UNITY_DIR)/extras/fixture/src \
         -I$(UNITY_DIR)/extras/memory/src \
         -Wall -Wextra -g

# 🔹 Добавьте unity_fixture.c в список исходников Unity
UNITY_SRC = $(UNITY_DIR)/src/unity.c \
            $(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
            $(UNITY_DIR)/extras/memory/src/unity_memory.c

# Заголовочные файлы (для зависимостей, если нужно)
UNITY_HDR = $(UNITY_DIR)/src/unity.h \
            $(UNITY_DIR)/src/unity_internals.h \
            $(UNITY_DIR)/extras/fixture/src/unity_fixture.h \
            $(UNITY_DIR)/extras/memory/src/unity_memory.h

# Список тестовых файлов
TEST_SRCS = main_test.c test_reader.c test_parser.c
# test_format.c test_estimator.c

# Исходники тестируемых модулей (только не зависящие от HAL)
APP_SRCS = ../Core/Src/reader.c ../Core/Src/parser.c
# ../Core/Src/format.c ../Core/Src/parser.c

# MOCK_SRCS = mocks/hal_mock.c

# 🔹 Объединяем все исходники: тесты, приложение, Unity (включая fixture)
SRCS = $(TEST_SRCS) $(APP_SRCS) $(UNITY_SRC) # $(MOCK_SRCS)
OBJS = $(SRCS:.c=.o)
TARGET = test_runner.exe

all: clean test

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	del /Q *.o $(TARGET) 2>nul
	del /Q $(UNITY_DIR)\src\*.o 2>nul
	del /Q $(UNITY_DIR)\extras\fixture\src\*.o 2>nul