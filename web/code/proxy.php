<?php
if(!isset($_GET['url'])){
	die('Invalid URL');
}
$url = rawurlencode($_GET['url']);
$content = file_get_contents("http://127.0.0.1:8881/$url");

if(FALSE === $content){
	header('HTTP/1.0 404 Not Found'); 
}else{
	echo $content;
}