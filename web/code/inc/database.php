<?php
if(!mysql_connect($_SERVER['OPENSHIFT_MYSQL_DB_HOST'],
	$_SERVER['OPENSHIFT_MYSQL_DB_USERNAME'],
	$_SERVER['OPENSHIFT_MYSQL_DB_PASSWORD'])
	){
	echo 'Can not connect to mysql!';
	throw new Exception('Can not connect to mysql!');
}
mysql_select_db('bsoj');
mysql_set_charset('utf8');
?>
