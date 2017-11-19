FROM jortizpa/oor-req:latest
MAINTAINER jose.orpa@gmail.com
RUN apt-get update && apt-get install software-properties-common && apt-add-repository ppa:ansible/ansible && apt-get update && apt-get install ansible && apt-get autoremove
RUN echo 'net.ipv4.conf.default.rp_filter=0' >> /etc/sysctl.conf
RUN echo 'net.ipv4.conf.all.rp_filter=0' >> /etc/sysctl.conf
RUN echo 'net.ipv4.ip_forward=1' >> /etc/sysctl.conf
RUN echo 'net.ipv6.conf.all.forwarding=1' >> /etc/sysctl.conf
RUN git clone git://github.com/OpenOverlayRouter/oor.git
WORKDIR /oor
RUN make
RUN make install
RUN rm -rf /oor
COPY files/*.conf /etc/
COPY files/*.sh /tmp/
COPY files/*.yaml /tmp/
ENV DEBUG 0
ENV OPMODE xTR
ENV IPMAPRESOLVER 147.83.131.32
ENV IPMAPSERVER 147.83.131.32
ENV KEYMAPSERVER upc_oor-password
ENV IPPROXYETRV4 193.162.145.46
ENV IPPROXYETRV6 193.162.145.46
ENV IPV4EIDPREFFIX ERROR
ENV IPV6EIDPREFFIX ERROR
CMD /tmp/start.sh $DEBUG $OPMODE $IPMAPRESOLVER $IPMAPSERVER $KEYMAPSERVER $IPPROXYETRV4 $IPPROXYETRV6 $IPV4EIDPREFFIX $IPV6EIDPREFFIX
