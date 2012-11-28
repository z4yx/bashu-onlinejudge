<?php
if(!isset($_GET['user_id']))
	die('Wrong argument.');

require('inc/database.php');
$user=mysql_real_escape_string($_GET['user_id']);

$query="select email,ip,accesstime,school,reg_time,submit,solved from users where user_id='$user'";
$row=mysql_fetch_row(mysql_query($query));

if(isset($_GET['type'])&&$_GET['type']=='json'){
	if(!$row)
		echo '{"nobody":0}';
	else{
		$failed=$solved="{";
		$res=mysql_query("select problem_id,min(result) from solution where user_id='$user' group by problem_id");
		while($row=mysql_fetch_row($res)){
			$id=$row[0];
			if($row[1]==0)
				$solved.="\"$id\":0,";
			else
				$failed.="\"$id\":0,";
		}
		echo '{"solved":',rtrim($solved,','),'},"failed":',rtrim($failed,','),'}}';
	}
}else{
	if(!$row)
		die('No such user');
	session_start();
	header('Content-Type: text/html; charset=utf-8');
?>
<table class="table table-condensed" style="margin-bottom:0px;">
	<colgroup>
		<col style="width:12%">
		<col style="width:5%">
		<col style="width:83%">
	</colgroup>
	<tbody>
	<tr><td colspan="2">User:</td><td><?php echo $user;?></td></tr>
	<tr><td colspan="2">Last Login:</td><td><?php echo $row[2];?></td></tr>
<?php if(isset($_SESSION['administrator'])){?>
	<tr><td colspan="2">IP:</td><td><?php echo $row[1];?></td></tr>
<?php }?>
	<tr><td colspan="2">School:</td><td><?php echo htmlspecialchars($row[3]);?></td></tr>
	<tr><td colspan="2">E-mail:</td><td><?php echo htmlspecialchars($row[0]);?></td></tr>
	<tr><td colspan="2">Reg Time:</td><td><?php echo $row[4];?></td></tr>
	<tr><td colspan="2">AC/Submit:</td><td><?php echo $row[6],'/',$row[5];?></td></tr>
	<?php
		$i=0;
		$failed=mysql_query("select problem_id from solution where user_id='$user' group by problem_id having min(result)>0");
		$number=mysql_num_rows($failed);
		echo "<tr><td>Failed:<br>($number)</td><td colspan=\"2\"><samp>";
		while($row=mysql_fetch_row($failed)){
			echo '<a target="_blank" href="problempage.php?problem_id=',$row[0],'">',$row[0],'</a>&nbsp;';
			if((++$i)==11){
				echo '<br>';
				$i=0;
			}
		}
		echo '</samp></td></tr>';
		$i=0;
		$solved=mysql_query("select problem_id from solution where result=0 and user_id='$user' group by problem_id");
		$number=mysql_num_rows($solved);
		echo "<tr><td>Sloved:<br>($number)</td><td colspan=\"2\"><samp>";
		while($row=mysql_fetch_row($solved)){
			echo '<a target="_blank" href="problempage.php?problem_id=',$row[0],'">',$row[0],'</a>&nbsp;';
			if((++$i)==11){
				echo '<br>';
				$i=0;
			}
		}
	?>
	</spam></td></tr>
	</tbody>
</table>
<?php
}
?>