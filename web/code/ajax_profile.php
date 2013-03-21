<?php
if(!isset($_POST['type'],$_POST['nick'],$_POST['email'],$_POST['school']))
	die('Invalid argument.');
if(strlen($_POST['nick'])>190)
	die('Nick is too long!');
if(strlen($_POST['school'])>60)
	die('School name is too long!');
if(strlen($_POST['email'])>60)
	die('E-mail is too long!');
if($_POST['type']=='profile'){
	if(!isset($_POST['oldpwd']))
		die('Invalid argument.');
	session_start();
	if(!isset($_SESSION['user']))
		die('Not logged in.');
	$user=$_SESSION['user'];
	require('inc/database.php');
    require_once('inc/checkpwd.php');
    if(!password_right($user, $_POST['oldpwd']))
		die('Old password is not correct!');
	
	$query='update users set email=\''.mysql_real_escape_string($_POST['email']).'\',school=\''.mysql_real_escape_string($_POST['school']).'\',nick=\''.mysql_real_escape_string($_POST['nick']).'\'';
	if(isset($_POST['newpwd']) && $_POST['newpwd']!=''){
		$len=strlen($_POST['newpwd']);
		if($len<6||$len>20)
			die('Password is too long or too short!');
		$query.=',password=\''.mysql_real_escape_string(my_rsa($_POST['newpwd'])).'\'';
	}
	$query.=" where user_id='$user'";
	mysql_query($query);
	echo "User infomation updated successfully!";
}else if($_POST['type']=='reg'){
	if(!isset($_POST['userid'],$_POST['newpwd']))
		die('Invalid argument.');
	require('inc/database.php');
	$user=mysql_real_escape_string(trim($_POST['userid']));
	$len=strlen($user);
	if($len==0)
		die('User name cannot be empty.');
	if($len>20)
		die('User name is too long.');
	if(preg_match('/\W/',$user))
		die('User name can only contain number,letter and "_".');

	$len=strlen($_POST['newpwd']);
	if($len<6||$len>20)
		die('Password is too long or too short!');
	$pwd=mysql_real_escape_string($_POST['newpwd']);

	mysql_query("insert into users (user_id,email,password,reg_time,nick,school) values ('$user','".mysql_real_escape_string($_POST['email'])."','$pwd',NOW(),'".mysql_real_escape_string($_POST['nick'])."','".mysql_real_escape_string($_POST['school'])."')");
	$code=mysql_errno();
	if($code==0)
		echo 'User created successfully!';
	else if($code==1062)
		echo "User '$user' exists.";
	else 
		echo "Unknown error.";
}
?>