<?php
if(!isset($_GET['url'])){
	die('Invalid URL');
}
$daemon_ip=$_SERVER["OPENSHIFT_INTERNAL_IP"];
$daemon_port=31415;

$url = rawurlencode($_GET['url']);
$content = file_get_contents("http://$daemon_ip:$daemon_port/$url");

if(FALSE === $content){
	header('HTTP/1.0 404 Not Found'); 
}else{
	echo $content;
}