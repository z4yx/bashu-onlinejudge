<?php
session_start();
header('Content-Type: text/html; charset=utf-8');
//header('Cache-Control: max-age=155520000');
if(!isset($_GET['mail_id']))
	die('Wrong argument.');
$mail=intval($_GET['mail_id']);
if(!isset($_SESSION['user']))
	die('Not Logged in');

require('inc/database.php');

$res=mysql_query('select content,new_mail from mail where UPPER(defunct)=\'N\' and mail_id='.$mail);
if($res && ($row=mysql_fetch_row($res))){
	echo htmlspecialchars($row[0]);
	if($row[1])
		mysql_query('update mail set new_mail=0 where mail_id='.$mail);
}
?>