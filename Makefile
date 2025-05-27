# Docker image name and tag
IMAGE_NAME = ffmpeg-trimmer
TAG = latest

# Container name
CONTAINER_NAME = ffmpeg-trimmer

# Directories
INPUT_DIR = ./input
OUTPUT_DIR = ./output

# Ensure the input and output directories exist
$(INPUT_DIR):
	mkdir -p $(INPUT_DIR)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# Build the Docker image
.PHONY: build
build:
	docker build -t $(IMAGE_NAME):$(TAG) .

# Run the container
.PHONY: run
run: $(INPUT_DIR) $(OUTPUT_DIR)
	docker run --rm \
		--name $(CONTAINER_NAME) \
		-v "$(INPUT_DIR):/app/input" \
		-v "$(OUTPUT_DIR):/app/output" \
		$(IMAGE_NAME):$(TAG)

# Run the container in interactive mode with a shell
.PHONY: shell
shell: $(INPUT_DIR) $(OUTPUT_DIR)
	docker run --rm -it \
		--name $(CONTAINER_NAME) \
		-v "$(INPUT_DIR):/app/input" \
		-v "$(OUTPUT_DIR):/app/output" \
		$(IMAGE_NAME):$(TAG) /bin/bash

# Stop any running containers
.PHONY: stop
stop:
	docker stop $(CONTAINER_NAME) || true

# Clean up: remove containers and images
.PHONY: clean
clean: stop
	docker rmi $(IMAGE_NAME):$(TAG) || true

# Clean up all: remove containers, images, and generated directories
.PHONY: clean-all
clean-all: clean
	rm -rf $(INPUT_DIR) $(OUTPUT_DIR)

# Show help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  build     - Build the Docker image"
	@echo "  run       - Run the container"
	@echo "  shell     - Run the container with an interactive shell"
	@echo "  stop      - Stop running containers"
	@echo "  clean     - Remove containers and images"
	@echo "  clean-all - Remove containers, images, and generated directories"
	@echo "  help      - Show this help message"

.DEFAULT_GOAL := help
