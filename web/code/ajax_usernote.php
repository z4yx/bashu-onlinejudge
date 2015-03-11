<?php
session_start();
if(!isset($_SESSION['user']))
    die('Not Logged in');
if(!isset($_POST['content'],$_POST['tags'],$_POST['problem_id']))
	die('Wrong argument.');

require('inc/database.php');
$content = mysql_real_escape_string($_POST['content']);
$tags = mysql_real_escape_string($_POST['tags']);
$problem_id = intval($_POST['problem_id']);
$user=$_SESSION['user'];

$res=mysql_query("INSERT INTO user_notes(problem_id,user_id,tags,content,edit_time) VALUES ($problem_id,'$user','$tags','$content',NOW())");
if(!$res){
    if(mysql_errno() == 1062){ //dup
        $res=mysql_query("UPDATE user_notes set tags='$tags',content='$content',edit_time=NOW() where problem_id=$problem_id and user_id='$user'");
    }
    if(!$res)
        die('error');
}
echo "__ok__";
?>