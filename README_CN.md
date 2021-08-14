# FRICC2介绍

FRICC2是PHP源码加密工具。当你使用PHP开发商业软件时可以使用FRICC2对发布的代码进行加密，加密后只有具备解密权限的PHP解析器才能执行程序。感谢PHP-Screw项目的贡献。





## 功能

1. 更少的文件句柄打开或读取(相比其他项目)。
2. 压缩支持，加密后文件更小(ZLIB低CPU占用率压缩，对嵌入式环境磁盘占用率更小)。
3. 解决数个内存泄漏问题(相比其他项目)。
4. 打开文件速度更快。
5. 支持以静态绑定方式编译到PHP解析器中(增加跟踪破解难度)。
6. 在Ubuntu / CentOS / OpenWRT等数个操作系统下测试完毕。
7. 在x86_64 / MIPS / ARM等数个不同架构CPU测试完毕。
8. 首个支持PHP 8(及JIT模式)，并经过内存泄漏检测的开源加密算法。





## 基本需求

### PHP版本

| PHP 版本          | 状态                | 备注                                          |
| ----------------- | ------------------- | --------------------------------------------- |
| PHP 8.X           | 静态扩展 / 动态扩展 | 推荐                                          |
| PHP 7.4.X         | 静态扩展 / 动态扩展 | 推荐                                          |
| PHP 7.0.X / 7.3.X | 静态扩展 / 动态扩展 | 运行时存在少量固定内存泄漏(PHP退出后自动清理) |
| 其他版本          | 未测试              | 未测试                                        |

### PHP的ZLIB支持

PHP需要编译时支持"--with-zlib"选项。





## 安装

### 获得源代码

```bash
# 从GIT获得源代码
cd ~/
git clone https://github.com/hoowa/FRICC2.git

# 可选的初始化KEY (使用不同的KEY更难破解)
chmod +x ~/FRICC2/fricc2load/init_key
~/FRICC2/fricc2load/init_key

# 基本依赖安装示例 CentOS 7.9
yum groupinstall "Development Tools"
yum install zlib-static zlib-devel

# 基本依赖安装示例 20.04
sudo apt install build-essential pkg-config autoconf zlib1g-dev
```

### 编译PHP扩展支持 (动态扩展)

```bash
# 编译 fricc2load
cd ~/FRICC2/fricc2load/
phpize
./configure
make
make install

# 在php.ini中增加如下
extension=fricc2load.so

# CLI命令下测试
php -m|grep fricc2load

# 重启web服务器或php服务即可生效
```

### 编译生成加密工具

```bash
# 编译生成加密工具
cd ~/FRICC2/fricc2/
make
cp fricc2 /usr/bin/

# 进行测试test.php
<?php
	echo "hello fricc2\n";
?>
fricc2 -o test_new.php test.php
php ./test_new.php
# Screen output:
hello fricc2
```



## 其他

### 可选编译PHP扩展支持 (静态扩展)

```bash
# 假如你的PHP在如下地址 ~/php-7.4.22/
cd ~/php-7.4.22
cp -avf ~/FRICC2/fricc2load/ ./ext/
./buildconf --force

# 在configure的时候增加此选项 --enable-fricc2load=static --with-zlib
./configure --enable-fricc2load=static --with-zlib
make
make install

# 检测扩展
php -m|grep fricc2load
```

### 作者

Sun Bing <hoowa.sun@gmail.com>

