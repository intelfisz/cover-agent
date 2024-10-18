# NOTE:
# If this is run behind a proxy server, remember to run docker build command
# with e.g. --build-arg http_proxy=$http_proxy etc.

# STAGE 1: build cover-agent from sources
FROM python:3.12-bullseye AS cc_exec

WORKDIR /app

# Copy all files
COPY . .

# Install required packages
RUN pip install poetry wandb
RUN poetry install

# Build cover agent executable (in dist folder)
RUN ["make", "installer"]

# Optional cmd if this image is to be used interactively
CMD ["/bin/bash"]

# STAGE 2: create environment for cover-agent usage for C
FROM python:3

# Install necessary packages including lcov for coverage reporting
RUN apt-get update && \
    apt-get install -y curl unzip ruby lcov
RUN pip install lcov_cobertura setuptools

# Set up the working directory
WORKDIR /usr/src/myapp

# Download and unzip Unity from GitHub
RUN curl -L https://github.com/ThrowTheSwitch/Unity/archive/master.zip -o unity-master.zip && \
    unzip unity-master.zip && \
    mv Unity-master Unity && \
    rm unity-master.zip

# Copy project files
COPY ./templated_tests/c_cli/. .

# Copy cover-agent executable
COPY --from=cc_exec /app/dist/cover-agent /usr/local/bin/cover-agent

# Build and run the tests with coverage
RUN chmod +x build_and_test_with_coverage.sh && ./build_and_test_with_coverage.sh

CMD ["/bin/bash"]





