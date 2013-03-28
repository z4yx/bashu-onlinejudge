<?php
function UserExist($uid){
	if(preg_match('/\W/',$uid))
		return false;
	$res=mysql_query("select user_id from users where user_id='$uid'");
	if($res && mysql_num_rows($res))
		return true;
	return false;
}
session_start();
header('Content-Type: text/html; charset=utf-8');
if(!isset($_SESSION['user']))
	die('Not Logged in');
if(!isset($_GET['op']))
	die('Not supported');
$op = $_GET['op'];

require('inc/database.php');
require('inc/mail_flags.php');

if($op=='check'){
	$uid=$_SESSION['user'];
	$res=mysql_query("select sum(new_mail) from mail where UPPER(defunct)='N' and to_user='$uid'");
	if($res && ($row=mysql_fetch_row($res)) && $row[0])
		echo $row[0];
	else
		echo '0';
}else if($op=='send'){
	$from=$_SESSION['user'];

	if(!isset($_POST['touser']) || strlen($touser=mysql_real_escape_string(trim($_POST['touser'])))==0)
		die('"To User" can\'t be empty.');
	if(!UserExist($touser))
		die('No such user.');
	if(!isset($_POST['title']) || strlen($title=mysql_real_escape_string(trim($_POST['title'])))==0)
		die('Title can\'t be empty.');
	if(isset($_POST['detail']))
		$detail=mysql_real_escape_string($_POST['detail']);
	else
		$detail='';
	mysql_query("insert into mail (from_user,to_user,title,content,in_date) VALUES ('$from','$touser','$title','$detail',NOW())");

	echo '__OK__';
}else{
	if(!isset($_GET['mail_id']))
		die('Wrong argument.');
	$mail=intval($_GET['mail_id']);

	if($op=='show'){
		$res=mysql_query("select content,new_mail,to_user from mail where UPPER(defunct)='N' and mail_id=$mail");
		if($res && ($row=mysql_fetch_row($res))){
			if(strcmp($_SESSION['user'], $row[2]))
				exit(0);
			echo htmlspecialchars($row[0]);
			if($row[1])
				mysql_query("update mail set new_mail=0 where mail_id=$mail");
		}
	}else if($op=='delete'){
		$res=mysql_query("select to_user from mail where UPPER(defunct)='N' and mail_id=$mail");
		if($res && ($row=mysql_fetch_row($res))){
			if(strcasecmp($row[0],$_SESSION['user'])==0)
				mysql_query("update mail set defunct='Y' where mail_id=$mail");
		}
	}else if($op=='star'){
		$uid=$_SESSION['user'];
		$mask=MAIL_FLAG_STAR;
		mysql_query("update mail set flags=(flags ^ $mask) where to_user='$uid' and mail_id=$mail");
		if(mysql_affected_rows()==1)
			echo  '__OK__';
	}
}
