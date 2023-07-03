#/usr/bin/env bash

# Use an Ubuntu 22 AWS VM, for example spawned by cf-remote:
# cf-remote spawn --count 1 --platform ubuntu-22-04-x64 --role hub --name ubuntu-22-fuzz-json

# To see where to SSH:
# cf-remote show

# When done, destroy the VM with:
# cf-remote destroy ubuntu-22-fuzz-json

# Run this script as root on an AWS Ubuntu 22 VM, you can curl it from GitHub:
# sudo bash
# curl -L -s https://raw.githubusercontent.com/olehermanse/json-experiment/main/fuzz.sh | bash

# See the end of this script, or the output for how to start fuzzing.

apt update -y
apt upgrade -y
apt-get install -y python3 python3-pip
apt-get install -y autoconf libtool make flex bison gdb
apt-get install -y liblmdb-dev libssl-dev libpcre3-dev libpam0g-dev

cd /home/ubuntu/

git clone https://github.com/mirrorer/afl.git
git clone https://gitlab.com/rc0r/afl-utils
git clone --recursive https://github.com/olehermanse/json-experiment.git

cd afl
make -j16
make install
cd ..

export CC=afl-gcc

echo core >/proc/sys/kernel/core_pattern

cd afl-utils
git checkout experimental
# TODO: Switch to a more "stable" version when another one is released
#       at the time of writing, v1.35a, is the latest and does not work.
python3 setup.py install
cd ..

cd json-experiment
./autogen.sh --enable-debug
make -j16
make install
cd ..

echo '{
  "input": "./afl_inputs",
  "output": "./afl_outputs",
  "target": "/usr/local/bin/json_manip_bin",
  "cmdline": "@@"
}
' > afl.conf

rm -rf /home/ubuntu/afl_outputs/
mkdir /home/ubuntu/afl_inputs/
mkdir /home/ubuntu/afl_outputs/

cp /home/ubuntu/json-experiment/examples/*.json /home/ubuntu/afl_inputs/

echo "To start fuzzing, run:"
echo "  afl-multicore -c afl.conf start 8"
echo "To see status, run:"
echo "  watch afl-whatsup afl_outputs/"

# Parallell fuzzing, using afl-multicore (afl-utils):
# afl-multicore -c afl.conf start 8

# Show stats:
# watch afl-whatsup afl_outputs/

# Collect crashes from multiple fuzzers:
# afl-collect -r afl_outputs/ ./collection -- /usr/local/bin/json_manip_bin --log-level CRITICAL -c @@

# Minimize corpus (remove "duplicates", reduce sizes of inputs)
# WARNING: VERY TIME CONSUMING
# afl-minimize -c new_corpus --cmin --cmin-mem-limit=500 --tmin --tmin-mem-limit=500 -j 8 ./afl_outputs/ -- /usr/local/bin/json_manip_bin @@
