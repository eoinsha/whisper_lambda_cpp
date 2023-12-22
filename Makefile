docker_build: Dockerfile *.cpp CMakeLists.txt
	docker build -t whisper_lambda_cpp .

binary: docker_build
	mkdir -p output
	docker run -v /tmp:/output whisper_lambda_cpp base64 /app/build/whisper_lambda_cpp > output/whisper_lambda_cpp.b64
	cat output/whisper_lambda_cpp.b64 | base64 -d > output/whisper_lambda_cpp
	rm output/whisper_lambda_cpp.b64

build-WhisperCppLambdaFunction: binary
	cp output/whisper_lambda_cpp $(ARTIFACTS_DIR)/bootstrap
