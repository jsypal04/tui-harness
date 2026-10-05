
BUILD_DIR = build
BIN = $(BUILD_DIR)/harness

CJSON = cjson/build/libcjson.so

all: $(BIN)

run: $(BIN)
	LD_LIBRARY_PATH=cjson/build ./$(BIN)

$(BIN): $(BUILD_DIR) $(CJSON)
	g++ -lcjson -o $@ main.cc

$(CJSON):
	cd cjson && make

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
