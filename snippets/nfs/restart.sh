#!/bin/sh

systemctl restart rpcbind.service
systemctl restart rpcbind.socket
systemctl restart nfs-server.service
systemctl restart nfs-utils.service


