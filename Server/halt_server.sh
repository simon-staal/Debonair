#!/bin/bash

set -euo pipefail

if [[ $# -eq 0 ]] ; then
    >&2 echo "Usage: ./halt_server.sh <IP>"
    >&2 echo "<IP> is the IP of the instance being connected to"
    exit 1
fi

# Finds ssh key
KEY=$(echo *.pem)
echo "${KEY}"

# Error checking
if [[ -z "${KEY}" ]] ; then
  >&2 echo "Error: No ssh permission file could be found in directory"
  >&2 echo "       Please include the permission file to ssh into the desired instance"
  exit 1
fi

WS=" "
if [[ ${KEY} =~ $WS ]]; then
  >&2 echo "Error: Multiple ssh permission files found in directory"
  >&2 echo "       Please have only 1 permission file for the desired instance"
  exit 1
fi

# Formatting
TERMINAL_WIDTH=$(tput cols)
SEP=$(echo $(printf '=%.0s' $(eval "echo {1.."$(($TERMINAL_WIDTH))"}")))

echo "$SEP"
echo "Connecting to server instance"
ssh -A -i ${KEY} ubuntu@${1} << EOF
  #!/bin/bash
  set -euo pipefail
  echo "Connection successful"
  echo "$SEP"

  echo "Halting MQTT broker"
  screen -X -S "mqtt" quit
  echo "$SEP"

  echo "$SEP"
  echo "Halting REST web service"
  screen -X -S "rest" quit
  echo "$SEP"
  
  echo "Server processes halted, Done"
EOF
