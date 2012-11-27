<?php
session_start();
if(!isset($_SESSION['user']))
	die('Not Logged in');

require('inc/database.php');

$res=mysql_query('select sum(new_mail) from mail where UPPER(defunct)=\'N\' and to_user=\''.$_SESSION['user'].'\'');
if($res && ($row=mysql_fetch_row($res)) && $row[0])
	echo $row[0];
else
	echo '0';

