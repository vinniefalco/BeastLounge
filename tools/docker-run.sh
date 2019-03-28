#!/bin/sh
# Work around the port number in Heroku being assigned at container runtime


set -e
if [ -z "$PORT" ]; then
    PORT=8080
fi

sed -i 's/\<PORT_NUMBER\>/'"$PORT"'/' $2
$1 $2
