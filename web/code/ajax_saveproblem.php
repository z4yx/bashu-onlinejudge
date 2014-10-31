<?php
session_start();
require('inc/database.php');
if(!isset($_SESSION['user'],$_GET['prob'],$_GET['op']))
	exit();
$user=$_SESSION['user'];
$problem_id=intval($_GET['prob']);
if($_GET['op']=='rm_saved'){
	mysql_query("DELETE from saved_problem where user_id='$user' and problem_id=$problem_id");
}else if($_GET['op']=='add_saved'){
	mysql_query("INSERT into saved_problem set problem_id=$problem_id,user_id='$user',savetime=NOW()");
}
if(mysql_affected_rows()===1)
	echo '__ok__';
