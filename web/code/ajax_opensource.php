<?php
if(!isset($_POST['id']))
	die('Invalid argument');
session_start();
if(!isset($_SESSION['user']))
	die('Not logged in');

require('inc/database.php');

$uid=($_SESSION['user']);
if('all'==$_POST['id']){
	mysql_query("update solution set public_code=1 where user_id='$uid'");
}else{
	$id=intval($_POST['id']);
	mysql_query("update solution set public_code=(!public_code) where solution_id=$id and user_id='$uid'");
	if(1==mysql_affected_rows())
		echo "success";
	else
		echo "failed";
}
?>