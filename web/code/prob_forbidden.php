    <div class="container" style="font-size:16px">
      <div class="row">
        <div class="span12" style="text-align:center;">
          Problem is not available!
        </div>
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
        var prob=<?php echo $prob_id?>;
        $('#nav_prob').parent().addClass('active');
        $('#ret_url').val("problempage.php?problem_id="+prob);
      });
    </script>
  </body>
</html>