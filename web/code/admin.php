<?php 
session_start();
?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Admin panel</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href="../assets/css/bootstrap.css" rel="stylesheet">
    <!--<link href="../assets/css/bootstrap-responsive.css" rel="stylesheet">-->
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
          
    <div class="container-fluid" style="font-size:13px">
      <div class="row-fluid">
<?php 
if(!isset($_SESSION['user']) || !isset($_SESSION['administrator'])){
       echo '<div class="center">You are not administrator.</div>';
}else{
  require('inc/database.php');
  if(isset($_POST['paswd'])){
    $uid=$_SESSION['user'];
    $res=mysql_query("select password from users where user_id='$uid'");
    if($row=mysql_fetch_row($res))
      if(strcmp($row[0], trim($_POST['paswd']))==0)
        $_SESSION['admin_panel']=1;
  }
  if(isset($_SESSION['admin_panel'])){
    $res=mysql_query('select content from news where news_id=0');
    $index_text=($res && ($row=mysql_fetch_row($res))) ? str_replace('<br>', "\n", $row[0]) : '';
?>
        <div class="span12">
          <div class="tabbable tabs-left">
            <ul class="nav nav-tabs" id="nav_tab">
              <li class="active"><a href="#tab_A" data-toggle="tab">Main Function</a></li>
              <li class=""><a href="#tab_B" data-toggle="tab">News</a></li>
              <li class=""><a href="#tab_C" data-toggle="tab">Contest</a></li>
              <li class=""><a href="#tab_D" data-toggle="tab">Privilege</a></li>
              <li class=""><a href="#tab_E" data-toggle="tab">Users</a></li>
            </ul>
            <div class="tab-content">
              <div class="tab-pane active" id="tab_A">
                <div class="row-fluid">
                  <div class="span3 offset1">
                    <a href="newproblem.php" style="margin-top:20px" class="btn btn-primary">Add Problem</a>
                  </div>
                  <div class="span5 offset1">
                    <h3 class="center">Home Page</h3>
                    <form action="#" method="post" id="form_index">
                      <input type="hidden" name="op" value="update_index">
                      <textarea name="text" rows="10" style="width:100%"><?php echo htmlspecialchars($index_text)?></textarea>
                      <div class="alert hide" id="alert_result">Updated successfully.</div>
                      <div class="pull-right">
                        <input type="submit" class="btn btn-small btn-primary" value="Update">
                      </div>
                    </form>
                  </div>
                </div>
              </div>
              <div class="tab-pane" id="tab_B">
                <div style="margin-left:50px;margin-right:50px">
                  <div id="table_news">
                    <div class="row-fluid">
                      <div class="alert span4">Loading news...</div>
                    </div>
                  </div>
                  <form action="admin.php" method="post" class="form-inline" id="form_news">
                    <label for="input_news" style="display:block">Add News</label>
                    <input type="text" id="input_news" name="news" class="input-xlarge" placeholder="Entering something...">
                    <input type="submit" class="btn" value="Add">
                    <input type="hidden" name="op" value="add_news">
                  </form>
                </div>
              </div>
              <div class="tab-pane" id="tab_C">
                <p>Developing...</p>
              </div>
              <div class="tab-pane" id="tab_D">
                <div style="margin-left:50px">
                  <div id="table_priv"></div>
                  <form action="admin.php" method="post" class="form-inline" id="form_priv">
                    <label for="input_user_id" style="display:block">Add Privilege</label>
                    <input type="text" id="input_user_id" name="user_id" class="input-small" placeholder="User ID">
                    <select name="right" id="slt_right">
                      <option value="administrator">administrator</option>
                      <option value="source_browser">source_browser</option>
                    </select>
                    <input type="submit" class="btn" value="Add">
                    <input type="hidden" name="op" value="add_priv">
                  </form>
                </div>
              </div>
              <div class="tab-pane" id="tab_E">
                <div style="margin-left:50px">
                  <div id="table_usr"></div>
                  <form action="admin.php" method="post" class="form-inline" id="form_usr">
                    <label for="input_dis_usr" style="display:block">Disable a user</label>
                    <input type="text" id="input_dis_usr" name="user_id" class="input-small" placeholder="User ID">
                    <input type="submit" class="btn" value="Disable">
                    <input type="hidden" name="op" value="disable_usr">
                  </form>
                </div>
              </div>
            </div>
          </div>
        </div>
<?php
  }else{?>
        <div class="span5 offset5">
          <form action="admin.php" class="form-inline" method="post">
            <div><label for="input_adminpass">Please enter your password</label></div>
            <input type="password" id="input_adminpass" name="paswd" class="input-small">
            <input type="submit" class="btn" value="Go">
          </form>
        </div>
<?php
  }
}
?>
      </div>
      <hr>
      <footer class="muted center" style="font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>

    </div>

    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap-tab.js"></script>
    <script src="../assets/js/bootstrap-modal.js"></script>
    <script src="../assets/js/bootstrap-dropdown.js"></script>
    <script src="common.js"></script>

    <script type="text/javascript">
      $(document).ready(function(){
        $('#ret_url').val("admin.php");

        var getprivlist=function(){$('#table_priv').load('ajax_admin.php',{op:'list_priv'});};
        var getnewslist=function(){$('#table_news').load('ajax_admin.php',{op:'list_news'});};
        var getusrlist=function(){$('#table_usr').load('ajax_admin.php',{op:'list_usr'});};
        $('#nav_tab').click(function(E){
          var jq=$(E.target);
          if(jq.is('a')){
            if(E.target.innerHTML.search(/Privilege/i)!=-1)
              getprivlist();
            else if(E.target.innerHTML.search(/News/i)!=-1)
              getnewslist();
            else if(E.target.innerHTML.search(/User/i)!=-1)
              getusrlist();
          }
        });
        $('#table_usr').click(function(E){
          E.preventDefault();
          var jq=$(E.target);
          if(jq.is('i')){
            var jq_uid=jq.parent().parent().prev(),oper;
            if(jq.hasClass('icon-remove')){
              jq_uid=jq_uid.prev();
              if(! window.confirm("Are you sure to delete "+jq_uid.html()))
                return false;
              oper='del_usr';
            }else{
              oper='en_usr';
            }
            $.ajax({
              type:"POST",
              url:"ajax_admin.php",
              data:{
                op:oper,
                user_id:jq_uid.html()
              },
              success:getusrlist
            });
          }
          return false;
        });
        $('#table_priv').click(function(E){
          E.preventDefault();
          var jq=$(E.target);
          if(jq.is('i')){
            var jq_pri=jq.parent().parent().prev();
            var jq_uid=jq_pri.prev();
            $.ajax({
              type:"POST",
              url:"ajax_admin.php",
              data:{
                op:'del_priv',
                user_id:jq_uid.html(),
                right:jq_pri.html()
              },
              success:getprivlist
            });
          }
          return false;
        });
        $('#form_usr').submit(function(E){
          E.preventDefault();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$('#form_usr').serialize(),
            success:getusrlist
          });
          return false;
        });
        $('#form_priv').submit(function(E){
          E.preventDefault();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$('#form_priv').serialize(),
            success:getprivlist
          });
          return false;
        });
        $('#form_news').submit(function(E){
          E.preventDefault();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$('#form_news').serialize(),
            success:getnewslist
          });
          return false;
        });
        $('#table_news').click(function(E){
          E.preventDefault();
          var jq=$(E.target);
          if(jq.is('i')){
            var jq_id=jq.parent().parent().prev().prev().prev();
            $.ajax({
              type:"POST",
              url:"ajax_admin.php",
              data:{
                op:'del_news',
                news_id:jq_id.html()
              },
              success:function(){jq_id.parent().remove();}
            });
          }
          return false;
        });
        $('#form_index').submit(function(E){
          E.preventDefault();
          $('#alert_result').hide();
          $.ajax({
            type:"POST",
            url:"ajax_admin.php",
            data:$('#form_index').serialize(),
            success:function(msg){
              if(/success/.test(msg))
                $('#alert_result').show();
            }
          });
          return false;
        });
      });
    </script>
  </body>
</html>