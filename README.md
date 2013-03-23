安装方法
===

Web
---

1. 安装并配置好一个可用的php服务器,如apache,nginx等,Linux用户建议安装php扩展[sysvsem]
2. 安装并启动mysql服务器,运行`doc/init.sql`初始化数据库,修改`web/code/inc/database.php`中的数据库连接信息
3. 如需显示数学公式,下载Mathjax并解压到`web/assets/Mathjax/`
4. 将web目录下的全部内容复制到网站根目录下,保证服务器用户(如www)有读写权限(770)
5. 测试能否访问http://localhost/code/index.php

Daemon
---

**Windows用户:**

1. 修改`daemon/windows_binary/config.ini`中的数据目录,mysql密码等信息
2. 运行`daemon/windows_binary/daemon.exe`
3. 如果提示"Started successfully.Waiting for submitting...",说明daemon启动成功,提交一个题目测试

**Linux用户:**

1. 安装编译工具,要求:
 - G++ >=4.6
 - libmicrohttpd >= 0.9.21
 - libmysqlclient 对应mysql版本
2. 在`daemon/`中运行

    `make`

3. 如果没有出错,修改`daemon/config.ini`中的数据目录,mysql密码等信息
4. 在`daemon/`中运行

    `./daemon`

5. 如果提示"Started successfully.Waiting for submitting...",说明daemon启动成功,提交一个题目测试

  [sysvsem]: http://php.net/manual/en/book.sem.php