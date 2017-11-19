#!/bin/bash
cat <<EOT >> /etc/ansible/hosts

[targets]

localhost              ansible_connection=local
EOT

sed -i s/DEBUG/$1/g /tmp/oor.yaml
sed -i s/OPMODE/$2/g /tmp/oor.yaml
sed -i s/IPMAPRESOLVER/$3/g /tmp/oor.yaml
sed -i s/IPMAPSERVER/$4/g /tmp/oor.yaml
sed -i s/KEYMAPSERVER/$5/g /tmp/oor.yaml
sed -i s/IPPROXYETRV4/$6/g /tmp/oor.yaml
sed -i s/IPPROXYETRV6/$7/g /tmp/oor.yaml
sed -i s/IPV4EIDPREFFIX/$8/g /tmp/oor.yaml
sed -i s/IPV6EIDPREFFIX/$9/g /tmp/oor.yaml
#ansible-playbook oor.yaml
#oor -f /etc/oor.conf
