FROM golang:1.19
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN go mod download && go mod verify
COPY test.png /usr/local/bin/
RUN go build -v -o /usr/local/bin/reader
CMD [ "reader"]