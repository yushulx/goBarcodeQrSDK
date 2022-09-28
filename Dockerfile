FROM golang:1.19
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp/example
RUN cp -R ../lib/ /usr/local/bin/
RUN cp test.png /usr/local/bin/
RUN go mod download
RUN go build -v -o /usr/local/bin/reader
CMD [ "reader"]