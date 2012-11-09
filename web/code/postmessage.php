<?php
require('inc/mutex.php');
function getNextMsgID(){
	$ID=1000;
	$res=mysql_query("select max(message_id) from message");
	if($res && ($r=mysql_fetch_row($res))){
		if($r[0])
			$ID=$r[0]+1;
	}
	return $ID;
}

session_start();
if(!isset($_SESSION['user']))
	die('Not Logged in');
$user_id=$_SESSION['user'];
$prob_id=0;
$msg_id=0;
$order_num=0;
$depth=0;

if(!isset($_POST['message']) || !isset($_POST['detail']))
	die('Wrong Argument');

require('inc/database.php');

$title=mysql_real_escape_string(trim($_POST['message']));
$title_len=strlen($title);
if($title_len==0)
	die('Message can\'t be empty');
if($title_len>500)
	die('Message is too long');

$content=mysql_real_escape_string($_POST['detail']);

$mutex=new php_mutex("/tmp/bsoj_postmessage.lock");
$new_msg_id=getNextMsgID();

if(isset($_POST['message_id'])
	&& ($tmp=intval($_POST['message_id']))
	&& ($row=mysql_fetch_row(mysql_query('select orderNum,depth,thread_id,problem_id from message where message_id='.$tmp)))
	){//Reply message

	$msg_id=$tmp;
	$order_num=$row[0];
	$depth=$row[1];
	$thread_id=$row[2];
	$prob_id=$row[3];

	$res=mysql_query("select depth,orderNum from message where thread_id=$thread_id and orderNum>$order_num order by orderNum");
	while($row=mysql_fetch_row($res)){
		if($row[0]<=$depth)
			break;
		$order_num=$row[1];
	}
	mysql_query("update message set orderNum=orderNum+1 where thread_id=$thread_id and orderNum>$order_num");
	mysql_query("update message set thread_id=$new_msg_id where thread_id=$thread_id");
	$depth++;
	$order_num++;
	
}else{//New message, check problem_id
	if(isset($_POST['problem_id'])){
		$tmp=intval($_POST['problem_id']);
		if(mysql_num_rows(mysql_query('select problem_id from problem where problem_id='.$tmp)))
			$prob_id=$tmp;
	}
}
mysql_query("insert into message (thread_id,message_id,parent_id,orderNum,problem_id,depth,user_id,title,content,in_date) values($new_msg_id,$new_msg_id,$msg_id,$order_num,$prob_id,$depth,'$user_id','$title','$content',NOW())");
$mutex->release_mutex();
header('location: board.php'.($prob_id ? "?problem_id=$prob_id" : ''));
//echo 'succeed';

?>