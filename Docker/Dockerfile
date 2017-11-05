FROM jortizpa/oor-docker-base:latest
MAINTAINER jose.orpa@gmail.com
WORKDIR /
COPY files/*.conf /etc/
COPY files/*.sh /tmp/
ENV IPV4EID ERROR
ENV IPV6EID ERROR
CMD /tmp/start.sh $IPV4EID $IPV6EID
