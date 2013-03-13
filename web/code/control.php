<?php
require('inc/checklogin.php');
 
if(!isset($_SESSION['user'])){
  $info='<div class="center">Not logged in.</div>';
}else{
  require('inc/database.php');
  $user_id=$_SESSION['user'];
}
?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Control panel</title>
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
        <div class="span6 offset3">
          <h3>Preferences</h3>
          <form id="form_preferences" action="ajax_preferences.php" method="post">
            <label class="checkbox">
              <input name="hidelogo" type="checkbox" <?php if($pref->hidelogo=='on')echo 'checked'; ?> > Hide logo
            </label>
            <input type="submit" class="btn" value="Save">
          </form>

          <h3>Backup my code</h3>
          <p>
            Download your last accepted submit of every problem.<br>You can do this only once a week.
            <?php
            if(!is_null($pref->backuptime))
              echo "<br><strong>Last Backup Time: ",date('Y-m-d H:i:s', $pref->backuptime),"</strong>";
            ?>
          </p>
          <!-- <button class="btn" id="create_btn">Create Backup</button> -->
          <button class="btn" id="download_btn">Backup &amp; Download</button>
        </div>
      <?php }?>
      </div>
      <hr>
      <footer class="muted center" style="font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>
    </div>

    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="common.js"></script>

    <script type="text/javascript"> 
      $(document).ready(function(){
        $('#form_preferences').submit(function(){
            $.ajax({
              type:"POST",
              url:"ajax_preferences.php",
              data:$('#form_preferences').serialize()
            });
            return false;
        });
        $('#download_btn').click(function(){
          $('body>iframe').remove();
          $('<iframe>').hide().attr('src','backupcode.php').appendTo('body');
        });
        $('#ret_url').val("control.php");
      });
    </script>
  </body>
</html>