<?php
require('inc/judge_backend.php');
if(!isset($_GET['url']) || 0!==preg_match('/\W/',$_GET['url'])){
	die('Invalid URL');
}
$url = rawurlencode($_GET['url']);
$content = file_get_contents("http://".JUDGE_DAEMON_IP.":".JUDGE_DAEMON_PORT."/$url");

if(FALSE === $content){
	header('HTTP/1.0 404 Not Found'); 
}else{
	echo $content;
}
