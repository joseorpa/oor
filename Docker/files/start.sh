#!/bin/bash
sed -i s/IPV4EID/$1/g /etc/oor.conf
sed -i s/IPV6EID/$2/g /etc/oor.conf
oor -f /etc/oor.conf
