
# hosts that will be used for library compilation
JOBS = 30
DISTCC_LIBS_HOSTS = "node-4/16 node-3/8 node-2/4 node-1/2 localhost/4"

# hosts that will be used for python wrapper compilation (needs lots of ram)
JOBS_PYTHON = 12
DISTCC_WRAPPERS_HOSTS = "node-4/8 node-3/4 node-2/2 node-1/1 localhost/1"

