<?php
require('inc/checklogin.php');
 
if(!isset($_SESSION['user'])){
  $info='<div style="text-align: center">Not logged in.</div>';
}else{
  require('inc/database.php');
  $user_id=$_SESSION['user'];
  $result=mysql_query('select email,nick,school from users where user_id=\''.$user_id."'");
  $row=mysql_fetch_row($result);
}
?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Profile</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href="../assets/css/bootstrap.css" rel="stylesheet">
    <link href="../assets/css/bootstrap-responsive.css" rel="stylesheet">
    <link href="../assets/css/docs.css" rel="stylesheet">
    <!--[if IE 6]>
    <link href="ie6.min.css" rel="stylesheet">
    <![endif]-->
    <!--[if lt IE 9]>
      <script src="../assets/js/html5.js"></script>
    <![endif]-->
  </head>

  <body>
    <?php require('page_header.php'); ?>  
          
    <div class="container-fluid">
      <div class="row-fluid">
      <?php
      if(isset($info)){
        echo $info;
      }else{
      ?>
        <div class="span8 offset3">
          <form class="form-horizontal" id="form_profile" action="#" method="post">
            <input type="hidden" value="profile" name="type">
            <fieldset>
              <div class="control-group">
                <label class="control-label">User Name</label>
                <div class="controls">
                  <span class="input-xlarge uneditable-input"><?php echo $user_id?></span>
                </div>
              </div>
              <div class="control-group" id="oldpwd_ctl">
                <label class="control-label" for="input_oldpwd">Old Password(*)</label>
                <div class="controls">
                  <input class="input-xlarge" id="input_oldpwd" name="oldpwd" type="password">
                </div>
              </div>
              <div class="control-group">
                <label class="control-label" for="input_nick">Nick Name</label>
                <div class="controls">
                  <input class="input-xlarge" type="text" name="nick" id="input_nick" value="<?php echo htmlspecialchars($row[1])?>">
                </div>
              </div>
              <div class="control-group" id="newpwd_ctl">
                <label class="control-label" for="input_newpwd">New Password</label>
                <div class="controls">
                  <input class="input-xlarge" type="password" id="input_newpwd" name="newpwd">
                  <br/><span>You can leave it blank.</span>
                </div>
              </div>
              <div class="control-group" id="reppwd_ctl">
                <label class="control-label" for="input_reppwd">Repeat Password</label>
                <div class="controls">
                  <input class="input-xlarge" type="password" id="input_reppwd">
                </div>
              </div>
              <div class="control-group">
                <label class="control-label" for="input_email">E-Mail</label>
                <div class="controls">
                  <input class="input-xlarge" type="text" name="email" id="input_email" value="<?php echo htmlspecialchars($row[0])?>">
                </div>
              </div>
              <div class="control-group">
                <label class="control-label" for="input_school">School</label>
                <div class="controls">
                  <input class="input-xlarge" type="text" name="school" id="input_school" value="<?php echo htmlspecialchars($row[2])?>">
                </div>
              </div>
              <div class="row-fluid">
                <div class="span9 center">
                  <span id="ajax_result" class="alert hide"></span>
                </div>
              </div>
              <div class="span3 offset3">
                <span id="save_btn" class="btn btn-primary">Save changes</span>
              </div>
            </fieldset>
          </form>
        </div>
      <?php } ?>
      </div>
      <hr>
      <footer class="muted" style="text-align: center;font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>
    </div>

    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="common.js"></script>

    <script type="text/javascript"> 
      $(document).ready(function(){
        $('#save_btn').click(function(){
          var b=false,pwd;
          if($('#input_oldpwd').attr('value')==''){
            $('#oldpwd_ctl').addClass('error');
            b=true;
          }else{
            $('#oldpwd_ctl').removeClass('error');
          }
          pwd=$('#input_newpwd').attr('value');
          if(pwd!='' && $('#input_reppwd').attr('value')!=pwd){
            b=true;
            $('#newpwd_ctl').addClass('error');
            $('#reppwd_ctl').addClass('error');
          }else{
            $('#newpwd_ctl').removeClass('error');
            $('#reppwd_ctl').removeClass('error');
          }
          if(!b){
            $.ajax({
              type:"POST",
              url:"ajax_profile.php",
              data:$('#form_profile').serialize(),
              success:function(msg){
                $('#ajax_result').html(msg).removeClass('alert-success').show();
                if(/success/.test(msg)){
                  $('#ajax_result').addClass('alert-success');
                }
              }
            });
          }
        });
        $('#ret_url').val("profile.php");
      });
    </script>
  </body>
</html>