<?php
if(!isset($_POST['uid']) || !isset($_POST['pwd']) || !isset($_POST['url']))
	die('Invalid argument.');
try{
	require('inc/database.php');
	$user=mysql_real_escape_string(trim($_POST['uid']));
	if(preg_match('/\W/',$user))
		throw new Exception('Login failed.');
	$res=mysql_query("select password,user_id,language from users where user_id='$user'");
	$r=mysql_fetch_row($res);
	if(!$r)
		throw new Exception("Login failed.");
	if(strcmp($r[0],$_POST['pwd'])!=0)
		throw new Exception("Login failed.");
	session_start();
	session_unset();
	$_SESSION['user']=$r[1];
	$_SESSION['lang']=$r[2];
	
	$res=mysql_query("select rightstr from privilege where user_id='$user'");
	while($r=mysql_fetch_row($res)){
		if($r[0]=='administrator' || $r[0]=='source_browser')
			$_SESSION[$r[0]]=true;
	}
	mysql_query("update users set accesstime=NOW() where user_id='$user'");
	//echo("Login succeeded.");
	header("location: ".$_POST['url']);
}catch(Exception $E){?>
<html><script language='javascript'>
	alert('UserName or Password Wrong!');
	history.go(-1);
</script></html>
<?php
}
?>