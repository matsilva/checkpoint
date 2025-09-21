# Checkpoint Project Makefile
# Convenience commands for project setup and development

.PHONY: help setup build clean debug release test format lint submodules

# Default target - show help
help:
	@echo "Checkpoint Project - Available targets:"
	@echo ""
	@echo "  setup         - Initialize project (submodules + build directory)"
	@echo "  build         - Build project in debug mode"
	@echo "  release       - Build project in release mode"
	@echo "  clean         - Remove build directories"
	@echo "  submodules    - Initialize/update git submodules"
	@echo "  format        - Run clang-format on source files"
	@echo "  example-config - Create example config.json file"
	@echo "  run           - Build and run checkpoint with config.jsonc"
	@echo "  run-with-config CONFIG=<path> - Run with specific config file"
	@echo "  stop          - Stop the running checkpoint server"
	@echo ""
	@echo "Quick start: make setup && make build && make run"

# Initialize project - get submodules and create build directory
setup: submodules
	@echo "Setting up project..."
	@mkdir -p build
	@cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -Wno-dev -DCMAKE_POLICY_VERSION_MINIMUM=3.5
	@echo "Setup complete! Run 'make build' to compile."

# Initialize/update submodules
submodules:
	@echo "Initializing git submodules..."
	@git submodule update --init --recursive
	@echo "Submodules initialized."

# Build in debug mode (default)
build:
	@if [ ! -d "build" ]; then \
		echo "Build directory not found. Running setup first..."; \
		$(MAKE) setup; \
	fi
	@echo "Building checkpoint (Debug)..."
	@cmake --build build -j$$(nproc)

# Build in release mode
release:
	@echo "Building checkpoint (Release)..."
	@mkdir -p build-release
	@cd build-release && cmake .. -DCMAKE_BUILD_TYPE=Release -Wno-dev
	@cmake --build build-release -j$$(nproc)

# Clean build artifacts
clean:
	@echo "Cleaning build directories..."
	@rm -rf build build-release
	@echo "Clean complete."

# Format source code
format:
	@echo "Formatting source files..."
	@find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
	@echo "Format complete."

# Create example config and directories if they don't exist
example-config:
	@if [ ! -f "config.jsonc" ]; then \
		echo "Creating config.jsonc from example..."; \
		cp example/config.jsonc config.jsonc; \
		echo "Created config.jsonc with default settings"; \
	fi
	@mkdir -p data
	@if [ ! -d "html" ] || [ -z "$$(ls -A html 2>/dev/null)" ]; then \
		echo "Copying HTML files..."; \
		mkdir -p html; \
		cp html/*.html html/ 2>/dev/null || cp html/*.woff html/ 2>/dev/null || true; \
		if [ ! -f "html/index.html" ]; then \
			echo "<html><body><h1>Checkpoint Protection Active</h1></body></html>" > html/index.html; \
		fi; \
	fi

# Build and run with config
run: build example-config
	@echo "Running checkpoint with config.jsonc..."
	@echo "Press Ctrl+C to stop the server"
	@./build/checkpoint -c config.jsonc

# Stop the running checkpoint server
stop:
	@echo "Stopping checkpoint server..."
	@pkill checkpoint || echo "No checkpoint process found"

# Run with specific config
run-with-config: build
	@if [ -z "$(CONFIG)" ]; then \
		echo "Usage: make run-with-config CONFIG=path/to/config.json"; \
		exit 1; \
	fi
	@./build/checkpoint $(CONFIG)