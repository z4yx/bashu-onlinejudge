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

**Windows用户:(Deprecated)**

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

配置文件
===


Daemon
---

####config.ini


* DATABASE_USER/DATABASE_PASS 为mysql数据库的连接用户名密码,密码可以留空
* datadir 为存放评测数据的目录,该目录存放以题号命名的目录,每个以题号命名的目录下存放该题的测试数据,测试数据输入输出文件名配对即可,例如:
	- /home/judge/data/1000/a1.in
	- /home/judge/data/1000/a1.out
	- /home/judge/data/1000/hello.in
	- /home/judge/data/1000/hello.out
	- /home/judge/data/1001/abc.in
	- /home/judge/data/1001/abc.out
	- /home/judge/data/1001/c2.in
	- /home/judge/data/1001/c2.out
* [lang*] 允许的提交语言及相应的编译选项,与web端的配置lang_conf.php对应

Web
---

####lang_conf.php


* LANG_NAME 设置各种语言的显示名称
* LANG_EXT 设置各种语言的源文件扩展名

####cookie.php

* <span style="color:red">cookie_key</span> 设置一个复杂的字符串用于对cookie加密,在生产环境中部署时务必修改,避免伪造cookie
* cookie_expire cookie的过期时间(用于记住登录)

####checklogin.php

* REQUIRE_AUTH 是否要求全局认证,即登录后才能参看OJ的题目等