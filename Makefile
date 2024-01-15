PROJECT_NAME = whisper_lambda_cpp
OUTPUT_DIR = output

.PHONY: docker_build binary build-bootstrap build-WhisperCppLambdaFunction build-WhisperCppLambdaFunctionArm

docker_build:
	@echo Building Docker Image for $(ARCH)
	docker build --platform=linux/$(ARCH) -t $(PROJECT_NAME)_$(ARCH) .

binary: docker_build
	mkdir -p $(OUTPUT_DIR)/$(ARCH)
	docker run -v /tmp:/$(OUTPUT_DIR) $(PROJECT_NAME)_$(ARCH) sh -c 'tar cf - $(PROJECT_NAME)* | base64' > $(OUTPUT_DIR)/artifacts_$(ARCH).b64
	cat $(OUTPUT_DIR)/artifacts_$(ARCH).b64 | base64 -d | tar xf - -C $(OUTPUT_DIR)/$(ARCH)

build-bootstrap: binary
	@echo Creating bootstrap artifacts in $(ARTIFACTS_DIR)
	cp $(OUTPUT_DIR)/$(ARCH)/$(PROJECT_NAME) $(ARTIFACTS_DIR)/bootstrap
	cp $(OUTPUT_DIR)/$(ARCH)/$(PROJECT_NAME)_test $(ARTIFACTS_DIR)/test_main
	chmod +x $(ARTIFACTS_DIR)/bootstrap
	chmod +x $(ARTIFACTS_DIR)/test_main

build-WhisperCppLambdaFunction:
	@echo Building for amd64
	$(MAKE) build-bootstrap ARCH=amd64

build-WhisperCppLambdaFunctionArm:
	@echo Building for Arm
	$(MAKE) build-bootstrap ARCH=arm64
