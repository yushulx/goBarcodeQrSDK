FROM golang:1.19
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp/example/command-line
RUN cp -r ../../lib/linux/* /usr/lib/x86_64-linux-gnu/
RUN cp template.json /usr/local/bin/
RUN go mod download
RUN go build -v -o /usr/local/bin/reader
CMD [ "reader"]