<?php
session_start();
require('inc/database.php');
require('inc/preferences.php');
if(!isset($_SESSION['user']))
	exit();
$user=$_SESSION['user'];
$pref=unserialize($_SESSION['pref']);

if(isset($_POST['hidelogo'])){
	$tmp='on';
}else{
	$tmp='off';
}
$pref->hidelogo=$tmp;
mysql_query("insert into preferences(user_id,property,value) values ('$user','hidelogo','$tmp') ON DUPLICATE KEY UPDATE value='$tmp'");

$_SESSION['pref']=serialize($pref);


