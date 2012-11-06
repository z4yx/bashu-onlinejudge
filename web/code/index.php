<?php 
function encode_user_id($user)
{
  if(!extension_loaded('openssl'))
    return false;
  $iv='7284565820000000';
  $key=hash('sha256','my)(password_xx0',true);
  return openssl_encrypt($user,'aes-256-cbc',$key,false,$iv);
}
session_start();
require('inc/database.php');
$res=mysql_query("select content from news where news_id=0");
$index_text=($row=mysql_fetch_row($res)) ? $row[0] : '';
?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Welcome to Bashu OnlineJudge</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <link href="../assets/css/bootstrap.css" rel="stylesheet">
    <link href="../assets/css/bootstrap-responsive.css" rel="stylesheet">
    <link href="../assets/css/docs.css" rel="stylesheet">

    <!-- Put the following javascript before the closing </head> tag. -->
    <script>
      (function() {
        var cx = '009214133664915278822:jxtzwpnxnz0';
        var gcse = document.createElement('script'); gcse.type = 'text/javascript'; gcse.async = true;
        gcse.src = (document.location.protocol == 'https:' ? 'https:' : 'http:') +
            '//www.google.com/cse/cse.js?cx=' + cx;
        var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(gcse, s);
      })();
    </script>
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
        <div class="span10 offset1">
          <div class="well" style="font-size:16px;padding:19px 40px">
            <?php echo $index_text?>
          </div>
        </div>
      </div>
      <div class="row-fluid">
        <div class="span5 offset1 well">
          <h4 class="center">News</h4>
          <?php
            $res=mysql_query("select DATE(time),title from news where news_id>0 order by news_id desc");
            if(!mysql_num_rows($res))
              echo '<h6 class="center muted" style="margin-top:20px">Nothing here</h4>';
            else{
              echo '<ul style="margin-bottom:0">';
              $i=0;
              while($i<13 && $row=mysql_fetch_row($res)){
                echo '<li>',$row[0],' ',$row[1],'</li>';
                $i++;
              }
              echo '</ul>';
              if($row=mysql_fetch_row($res)){
                echo '<a href="#"  style="margin-left:25px" id="a_more_chl">More...</a>',
                      '<ul style="margin-bottom:0" class="hide" id="more_chl">';
                do{
                  echo '<li>',$row[0],' ',$row[1],'</li>';
                }while($row=mysql_fetch_row($res));
                echo '</ul>';
              }
            }
          ?>
        </div>
        <div class="span5 well">
          <h4 class="center">Chat Room</h4>
          <div style="clear:both">
            <div class="pull-left">
              <button class="btn btn-small" id="btn_switch">Enter</button>
            </div>
            <div class="pull-right" style="color:#468847" id="info_socket"></div>
          </div>
          <textarea id="chat_content" rows="10" class="chat-content" readonly disabled></textarea>
          <input type="text" id="ipt_message" class="span9" style="margin-bottom:0" placeholder="Type message" disabled>
          <button class="btn btn-primary pull-right" disabled id="btn_send">Send</button>
        </div>
      </div>
      <div class="row-fluid">
        <div class="span10 offset1 well">
          <h4 class="center">Google Search</h4>
          <gcse:search></gcse:search>
        </div>
      </div>
      <hr>
      <footer class="muted" style="text-align: center;font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>
    </div>
    <script type="text/javascript">
      <?php
        echo 'var ws_url="ws://',$_SERVER["SERVER_ADDR"],':6844/";';
        if(isset($_SESSION['user']))
          echo 'var userid="',encode_user_id('id-'.$_SESSION['user']),'";';
      ?>
    </script>
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap-modal.js"></script>
    <script src="../assets/js/bootstrap-dropdown.js"></script>
    <script src="common.js"></script>
    <script src="../assets/js/chat.js"></script>
    <script type="text/javascript"> 
      $(document).ready(function(){
        $('#ret_url').val("index.php");

        $('#a_more_chl').click(function(E){
          $(E.target).remove();
          $('#more_chl').show();
        });
      });
    </script>
  </body>
</html>
