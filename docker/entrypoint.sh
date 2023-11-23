#!/bin/bash
# entrypoint.sh

# Source the sourceme.sh script
source /gap_sdk/sourceme.sh 1

# Execute the CMD (default command, which is /bin/bash in this case)
exec "$@"
