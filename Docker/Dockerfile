FROM jortizpa/oor-req:latest
MAINTAINER jose.orpa@gmail.com
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
ENV IPV4EID ERROR
ENV IPV6EID ERROR
CMD /tmp/start.sh $IPV4EID $IPV6EID
