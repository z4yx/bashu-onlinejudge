<?php 

require('inc/checklogin.php');
require('inc/database.php');

$cate_result=mysql_query('SELECT DISTINCT category from problem_category order by category');
$categories = array();
while($category_row=mysql_fetch_row($cate_result)){
  array_push($categories, $category_row[0]);
}


$Title="Problem Category";
?>
<!DOCTYPE html>
<html>

    <?php require('head.php'); ?>

  <body>
    <?php require('page_header.php'); ?>  
          
    <div class="container-fluid">
      <div class="row-fluid">
        <div class="offset2 span8" style="font-size:16px">
          <div class="page-header">
            <h2><?php echo count($categories)?> Categories</h2>
          </div>
          <p class="problem-category">
                <?php
                foreach ($categories as $key => $i)
                    echo '<a href="problemset.php?category=',htmlspecialchars($i),'"><span class="label label-success">',htmlspecialchars($i),'</span></a>';
                ?>
          </p>
        </div>
      </div>
      <hr>
      <footer>
        <p>&copy; 2012-2016 Bashu Middle School</p>
      </footer>
    </div>
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="../assets/js/common.js"></script>
    <script type="text/javascript"> 
      $(document).ready(function(){
        $('#nav_cate').parent().addClass('active');
        $('#ret_url').val("category.php");

      });
    </script>
  </body>
</html>
