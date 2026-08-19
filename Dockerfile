# Build stage
FROM golang:1.24 AS builder
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp/example/command-line
# Copy Dynamsoft shared libraries into system library path so the
# built goBarcodeQrSDK binaries can find them at runtime
RUN cp -r ../../dcv/lib/linux/* /usr/lib/x86_64-linux-gnu/
RUN cp ../../template.json /usr/local/bin/
RUN go mod download
RUN CGO_ENABLED=1 go build -v -o /usr/local/bin/reader .

# Runtime stage
FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
    libc6 libstdc++6 libgomp1 ca-certificates \
    && rm -rf /var/lib/apt/lists/*
COPY --from=builder /usr/local/bin/reader /usr/local/bin/reader
COPY --from=builder /usr/src/myapp/example/command-line/template.json /usr/local/bin/template.json
# Dynamsoft SDK needs its model files
COPY --from=builder /usr/src/myapp/dcv/lib/linux/Models /usr/local/share/dcv/Models
WORKDIR /usr/local/bin
ENTRYPOINT ["reader"]