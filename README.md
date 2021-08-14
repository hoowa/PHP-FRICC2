# What's FRICC2?

FRICC2 is a PHP Script encryption tool. When you are developing a commercial software using PHP, the script can be distributed as encrypted up until just before execution. Thanks for PHP-Screw.

[**中文版说明**](./README_CN.md)



## Features

1. Less file handle open or read.
2. Compress supported, small size of target files.
3. Fix serval memory Leak.
4. More performance when php open files.
5. Supported Bundle in PHP with static.
6. Tested on Ubuntu / CentOS / Openwrt.
7. Tested in x86_64 / MIPS / ARM Processors.





## Requirement

### PHP Version

| PHP Version       | Status          | Notice                                               |
| ----------------- | --------------- | ---------------------------------------------------- |
| PHP 8.X           | Static / Shared | Recommend                                            |
| PHP 7.4.X         | Static / Shared | Recommend                                            |
| PHP 7.0.X / 7.3.X | Static / Shared | Runtime Little Memory Leak (auto free when php exit) |
| Other Version     | NO TEST         | NO TEST                                              |

### ZLIB Support in PHP

PHP must be compiled with the "--with-zlib" option.





## Installation

### Get Source

```bash
# git clone source
cd ~/
git clone https://github.com/hoowa/FRICC2.git

# optional init key (Set Better Secure different)
chmod +x ~/FRICC2/fricc2load/init_key
~/FRICC2/fricc2load/init_key

# Requirement Example CentOS 7.9
yum groupinstall "Development Tools"
yum install zlib-static zlib-devel

# Requirement Example Ubuntu 20.04
sudo apt install build-essential pkg-config autoconf zlib1g-dev
```

### Build PHP Extension (Shared)

```bash
# Compile fricc2load
cd ~/FRICC2/fricc2load/
phpize
./configure
make
make install

# Append this line to php.ini
extension=fricc2load.so

# Test in CLI
php -m|grep fricc2load

# Restart the web server / your php daemon as you need
```

### Build Encode Program

```bash
# Compile Encode Program
cd ~/FRICC2/fricc2/
make
cp fricc2 /usr/bin/

# Test Create test.php
<?php
	echo "hello fricc2\n";
?>
fricc2 -o test_new.php test.php
php ./test_new.php
# Screen output:
hello fricc2
```



## Others

### Optional Build PHP Extension (Static)

```bash
# example of your php source in ~/php-7.4.22/
cd ~/php-7.4.22
cp -avf ~/FRICC2/fricc2load/ ./ext/
./buildconf --force

# append --enable-fricc2load=static --with-zlib to configure (example):
./configure --enable-fricc2load=static --with-zlib
make
make install

# check modules
php -m|grep fricc2load
```

### Author

Sun Bing <hoowa.sun@gmail.com>

