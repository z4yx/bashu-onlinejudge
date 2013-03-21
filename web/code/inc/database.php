<?php
if(!mysql_connect('localhost','root','1995714')){
	echo 'Can not connect to mysql!';
	throw new Exception('Can not connect to mysql!');
}
mysql_select_db('bsoj');
mysql_set_charset('utf8');
?>
