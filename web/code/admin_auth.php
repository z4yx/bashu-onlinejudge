<?php
require('inc/checklogin.php');

if(!isset($_SESSION['user'],$_SESSION['administrator']))
  die('<div class="center">You are not administrator.</div>');

require('inc/database.php');
if(isset($_POST['paswd'])){

  require_once('inc/checkpwd.php');
  if(password_right($_SESSION['user'], $_POST['paswd'])){
    $_SESSION['admin_tfa']=1;
    if(isset($_SESSION['admin_retpage']))
      $ret = $_SESSION['admin_retpage'];
    else
      $ret = "index.php";
    header("Location: $ret");
    exit(0);
  }
}

$Title="Admin Verification";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>
  <body>
    <?php require('page_header.php'); ?>  
          
    <div class="container-fluid admin-page">
      <div class="row-fluid">
      
        <div class="span5 offset5">
          <form action="admin_auth.php" class="form-inline" method="post">
            <div><label for="input_adminpass">Please enter your password</label></div>
            <input type="password" autofoucs id="input_adminpass" name="paswd" class="input-small">
            <input type="submit" class="btn" value="Go">
          </form>
        </div>
      </div>
      <hr>
      <footer>
        <p>&copy; 2012-2014 Bashu Middle School</p>
      </footer>

    </div>

    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="common.js"></script>

    <script type="text/javascript">
      $(document).ready(function(){
        $('#ret_url').val("admin_auth.php");
        $('#input_adminpass').focus();
      });

    </script>
  </body>
</html>