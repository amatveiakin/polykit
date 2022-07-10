FROM ubuntu:20.04

RUN apt update
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt install -y golang-go libtbb-dev liblinbox-dev libgmp-dev libntl-dev ca-certificates openssl clang gcc python3-dev vim

# Make `go install` happy with the certificates
ARG cert_location=/usr/local/share/ca-certificates
# Get certificate from "github.com"
RUN openssl s_client -showcerts -connect github.com:443 </dev/null 2>/dev/null|openssl x509 -outform PEM > ${cert_location}/github.crt
# Get certificate from "proxy.golang.org"
RUN openssl s_client -showcerts -connect proxy.golang.org:443 </dev/null 2>/dev/null|openssl x509 -outform PEM >  ${cert_location}/proxy.golang.crt
# Update certificates
RUN update-ca-certificates

RUN GO111MODULE=on go install github.com/bazelbuild/bazelisk
ENV PATH="/root/go/bin:$PATH"

RUN ln -s /usr/bin/python3 /usr/bin/python