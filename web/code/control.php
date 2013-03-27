<?php
require('inc/checklogin.php');
 
if(!isset($_SESSION['user'])){
  $info='<div class="center">Not logged in.</div>';
}else{
  require('inc/database.php');
  $user_id=$_SESSION['user'];
}
$Title="Control panel";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>
  <body>
    <?php require('page_header.php'); ?>  
          
    <div class="container-fluid control_panel">
      <div class="row-fluid">
      <?php
      if(isset($info)){
        echo $info;
      }else{
      ?>
        <div class="span6 offset3">
          <h2>Preferences</h2>
          <form id="form_preferences" action="ajax_preferences.php" method="post">
            <label class="checkbox">
              <input name="hidelogo" type="checkbox" <?php if($pref->hidelogo=='on')echo 'checked'; ?> > Hide logo
            </label>
            <label class="checkbox">
              <input name="sharecode" type="checkbox" <?php if($pref->sharecode=='on')echo 'checked'; ?> > Share my code by default
            </label>
            <input type="submit" class="btn" value="Save">
          </form>

          <h2>Backup my code</h2>
          <p>
            Download your last accepted submit of every problem.<br>You can do this only once a week.
            <?php
            if(!is_null($pref->backuptime))
              echo "<br><strong>Last Backup Time: ",date('Y-m-d H:i:s', $pref->backuptime),"</strong>";
            ?>
          </p>
          <button class="btn" id="download_btn">Backup &amp; Download</button>

          <h2>Open source</h2>
          <p>
            <strong>Why Should I Open Source My Codes?</strong>
            <ol>
              <li>Open source softwares are a kind of great cultures which can date back to the early ages of the computer science history, and they are also the foundation of the web, the Internet, and our world.</li>
              <li>If everybody shares his code with the world, it will give everyone chances to use the programs, distribute the programs, understand the programs and improve the programs, which helps the original author in turn.</li>
              <li>Codes in the OI are relatively short, but they can also be very obscure. Making them open-source is to help ourselves in the past time.</li>
            </ol>
          </p>
          <button class="btn" id="open_source">Open source all my code</button>
        </div>
      <?php }?>
      </div>
      <hr>
      <footer>
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
              success:function(msg){location.reload();},
              data:$('#form_preferences').serialize()
            });
            return false;
        });
        $('#download_btn').click(function(){
          $('body>iframe').remove();
          $('<iframe>').hide().attr('src','backupcode.php').appendTo('body');
        });
        $('#open_source').click(function(){
          if(!window.confirm("Are you sure?"))
            return false;
          $.post('ajax_opensource.php',{id:'all'});
        });
        $('#ret_url').val("control.php");
      });
    </script>
  </body>
</html>
