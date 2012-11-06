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
$index_text= ($row=mysql_fetch_row($res)) ? $row[0] : '';
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
          <h4 class="center">ChangeLog</h4>
          <ul style="margin-bottom:0">
            <li>2012-11-5 增加Google自定义搜索</li>
            <li>2012-11-5 开放注册</li>
            <li>2012-10-26 导航条自动置顶</li>
            <li>2012-10-22 board支持BBCode</li>
            <li>2012-10-22 record中改变过滤条件立即生效</li>
            <li>2012-10-22 problemset提交过的题目前的指示</li>
            <li>2012-10-21 Special Judge功能完成</li>
            <li>2012-10-19 board回复面板左下角调整大小</li>
            <li>2012-10-19 添加Logo,导航条取消fixed</li>
            <li>2012-10-18 Board有内容的主题前添加加号</li>
            <li>2012-10-17 增加User compare功能</li>
            <li>2012-10-16 网站针对IE6弹出提示</li>
            <li>2012-10-16 用户信息中显示"Tried but failed"题目</li>
          </ul>
          <a href="#"  style="margin-left:25px" id="a_more_chl">More...</a>
          <ul style="margin-bottom:0" class="hide" id="more_chl">
            <li>2012-10-15 主页上的聊天室完成</li>
            <li>2012-10-12 修改界面配色</li>
            <li>2012-10-12 Bootstrap库升至2.1</li>
            <li>2012-10-10 题目页面允许隐藏右边栏</li>
            <li>2012-10-8 题目编辑页面增加上传图片功能</li>
            <li>2012-9-29 为手机等低分辨率设备作了界面调整</li>
            <li>2012-9-28 管理员用户菜单中添加管理员页面</li>
            <li>2012-9-28 修正题目编辑页面的小问题</li>
            <li>2012-9-27 Linux评测程序搭建完成</li>
            <li>2012-9-26 网页正式搬迁到Linux server</li>
            <li>2012-9-24 添加MathJax数学公式渲染引擎</li>
            <li>2012-9-22 修复record页面翻页问题</li>
            <li>2012-9-22 修复一个可能引起许多表单失效的重大bug</li>
            <li>2012-9-22 增加分享代码功能</li>
            <li>2012-9-20 First preview!</li>
          </ul>
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
