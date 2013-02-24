<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Login</title>
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
    <style type="text/css">
    .fake-legend{border-bottom-color:#e5e5e5;border-top:0;}
    #top_title{margin: 50px auto;}
    </style>
  </head>

  <body>
          
    <div class="container-fluid">
      <div class="row-fluid">
        <div class="span12 center">
          <h1 id="top_title"></h1>
        </div>
      </div>
      <div id="loginpage" class="row-fluid">
        <div style="width:560px;margin:0 auto;">
          <form class="form-horizontal well" id="form_login" action="login.php" method="post">
            <h1 class="center">Authorization Required</h1>
            <hr class="fake-legend">
            <div id="uid_ctl" class="control-group">
              <label class="control-label" for="uid">Name:</label>
              <div class="controls">
                <input autofocus="autofocus" type="text" id="uid" name="uid" placeholder="Username">
              </div>
            </div>
            <div id="pwd_ctl" class="control-group">
                <label class="control-label" for="pwd">Password:</label>
                <div class="controls">
                  <input id="pwd" name="pwd" type="password" placeholder="Password">
                </div>
            </div>
            <div class="control-group">
              <div class="controls">
                <label class="checkbox">
                  <input type="checkbox" name="remember">&nbsp;Remember me
                </label>
                <input id="signin" type="submit" value="Sign in" class="btn btn-primary">
                <a href="#" onclick="return switch_page();" style="line-height:30px">Sign up</a>
              </div>
            </div>
            <input id="ret_url" name="url" value="index.php" type="hidden">
          </form>
        </div>
      </div>

      <div id="regpage" class="hide row-fluid">
        <div class="span6 offset3">
          <form class="form-horizontal well" id="form_profile" action="#" method="post">
            <input type="hidden" value="reg" name="type">
            <fieldset>
              <div class="control-group" id="userid_ctl">
                <label class="control-label">User Name</label>
                <div class="controls">
                  <input class="input-xlarge" type="text" name="userid" id="input_userid">
                </div>
              </div>
              <div class="control-group">
                <label class="control-label" for="input_nick">Nick Name</label>
                <div class="controls">
                  <input class="input-xlarge" type="text" name="nick" id="input_nick">
                </div>
              </div>
              <div class="control-group" id="newpwd_ctl">
                <label class="control-label" for="input_newpwd">Password</label>
                <div class="controls">
                  <input class="input-xlarge" type="password" id="input_newpwd" name="newpwd">
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
                  <input class="input-xlarge" type="text" name="email" id="input_email">
                </div>
              </div>
              <div class="control-group">
                <label class="control-label" for="input_school">School</label>
                <div class="controls">
                  <input class="input-xlarge" type="text" name="school" id="input_school">
                </div>
              </div>
              <div class="center">
                <span id="save_btn" class="btn btn-primary">Submit</span>
              </div>
              <div class="row-fluid">
                <span id="ajax_result" class="hide span6 offset3 alert alert-error center" style="margin-top:20px"></span>
              </div>
            </fieldset>
          </form>
        </div>
      </div>

      <hr>
      <footer class="muted center" style="font-size:12px">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>
    </div>
    <script src="../assets/js/jquery.js"></script>
    <script src="common.js"></script>
    <script type="text/javascript">
      function switch_page() {
        $('#loginpage').hide();
        $('h1').html('Request Account');
        $('#regpage').show();
        return false;
      }
      $(document).ready(function() {
        $('#save_btn').click(function(){
          var b=false,pwd;
          if(!$.trim($('#input_userid').val())) {
            $('#userid_ctl').addClass('error');
            b=true;
          }else{
            $('#userid_ctl').removeClass('error');
          }
          pwd=$('#input_newpwd').val();
          if(pwd!='' && $('#input_reppwd').val()!=pwd){
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
                if(/created/.test(msg)){
                  window.alert('Your account request has been submitted,\nit will be reviewed by the administrator.');
                  window.location="index.php";
                }else{
                  $('#ajax_result').html(msg).show();
                }
              }
            });
          }
        });
      });
    </script>
  </body>
</html>
