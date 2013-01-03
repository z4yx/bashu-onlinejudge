<!--[if lt IE 9]>
  <script>window.fix_ie_pre=true;</script>
<![endif]-->
<!--[if IE 6]>
  <script>var LETSKILLIE6_DELAY=1;</script>
  <script src="http://letskillie6.googlecode.com/svn/trunk/2/zh_CN.js"></script>
<![endif]-->
<header>
  <a href="index.php"><img src="../assets/img/logo.jpg" alt="Logo" class="img-rounded"></a>
</header>
<div class="navbar" id="navbar_top">
  <div class="navbar-inner" style="padding:0">
    <div class="container-fluid navbar-padding-fix">
      <a class="brand" href="index.php">Bashu OnlineJudge</a>
      <div class="btn-group pull-right">

<?php if(isset($_SESSION['user'])){?>
        <a class="btn dropdown-toggle" data-toggle="dropdown" style="white-space:nowrap" href="#">
          <i class="icon-user"></i>
          <?php
          echo $_SESSION['user'],'<strong id="notifier"></strong>';
          ?>
          <span class="caret"></span>
        </a>
        <ul class="dropdown-menu">
          <li><a href="mail.php">Mail</a></li>
          <li><a href="profile.php">Profile</a></li>
<?php   if(isset($_SESSION['administrator']))
          echo '<li class="divider"></li><li><a href="admin.php">Admin</a></li>'; 
?>
          <li class="divider"></li>
          <li><a id='logoff_btn' href="#">Sign Out</a></li>
        </ul>
<?php }else{?>
        <a id="login_btn" data-toggle="modal" href="#LoginModal" class="btn">Login</a>
        <a href="reg.php" class="btn">Sign Up</a>
<?php }?>
      </div>
        <ul class="nav">
          <li><a id="nav_bbs" href="board.php">Board</a></li>
          <li><a id="nav_set" href="problemset.php">Problemset</a></li>
          <li><a id="nav_prob" href="problempage.php">Problem</a></li>
          <li><a id="nav_record" href="record.php">Record</a></li>
          <li><a id="nav_rank" href="ranklist.php">Ranklist</a></li>
          <li><a id="nav_about" href="about.php">About</a></li>
        </ul>
        <form class="navbar-search pull-left" id="search_form" action="search.php" method="get">
          <input type="text" name="q" id="search_input" class="search-query input-medium" style="margin-bottom:0px;width:auto;" autocomplete="off" placeholder="Problem ID,Title,Source">
        </form>
    </div>
  </div>
</div>    
<div class="modal hide" id="LoginModal">
  <form id="form_login" style="margin:0px" action="login.php" method="post">
    <div class="modal-header">
      <a class="close" data-dismiss="modal">×</a>
      <h4>Login Window</h4>
    </div>
    <div class="modal-body">
      <div id="uid_ctl" class="control-group">
        <label class="control-label" for="uid">Name:</label>
        <div class="controls">
          <input type="text" id="uid" name="uid" placeholder="Username">
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
        </div>
      </div>
      <input id="ret_url" name="url" type="hidden"><!--value=""-->
    </div>
    <div class="modal-footer">
      <input id="signin" type="submit" value="Sign in" class="btn btn-primary">
      <a href="#" class="btn" data-dismiss="modal">Cancel</a>
    </div>
  </form>
</div>
    
