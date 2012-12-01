<?php
if(!isset($_POST['uid']) || !isset($_POST['pwd']) || !isset($_POST['url']))
	die('Invalid argument.');
try{
	require('inc/database.php');
	$user=mysql_real_escape_string(trim($_POST['uid']));
	if(preg_match('/\W/',$user))
		throw new Exception('Invalid user ID');
	$res=mysql_query("select password,user_id,language,defunct from users where user_id='$user'");
	$r=mysql_fetch_row($res);
	if(!$r)
		throw new Exception("No such user");
	if($r[3]!='N')
		throw new Exception("User is disabled");
		
	if(strcmp($r[0],$_POST['pwd'])!=0)
		throw new Exception("Password is incorrect");
	session_start();
	session_unset();
	$_SESSION['user']=$r[1];
	$_SESSION['lang']=$r[2];
	
	$res=mysql_query("select rightstr from privilege where user_id='$user'");
	while($r=mysql_fetch_row($res)){
		if($r[0]=='administrator' || $r[0]=='source_browser')
			$_SESSION[$r[0]]=true;
	}
	$ip=mysql_escape_string($_SERVER["REMOTE_ADDR"]);
	mysql_query("update users set accesstime=NOW(),ip='$ip' where user_id='$user'");

	if(isset($_POST['remember'])){
		require_once 'inc/cookie.php';
		write_cookie();
	}
	//echo("Login succeeded.");
	header("location: ".$_POST['url']);
}catch(Exception $E){?>
<html><head><script language="javascript">
	alert('<?php echo $E->getMessage();?>');
	history.go(-1);
</script></head></html>
<?php
}
?>