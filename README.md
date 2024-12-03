# simplebench


## Install

**Only tested on Linux** 

```bash
make

cd build/
```

## Example Usage

```bash
build/simplebench -e 10 -f /dev/sdb1 -r 1024 -s 4 -t R -p R -q 2 -d T -o /tmp/test.tr
```

## Create a disk file

```bash
dd if=/dev/zero of=/tmp/foo bs=1048576 count=64
```
