<?php

   if(isset($_POST['BtnSubmit']))
   {
      echo "<h2>Bellow is the form data</h2>";
      echo "<hr>";
      echo "</br>Your Name :{$_POST['Fullname']}";
      echo "</br>Your Address :{$_POST['UserAddress']}";
      echo "<hr>";
   }

?>

<h2>PHP HTML Form textarea Example</h2>

<form name="UserInformationForm" method="POST" action="#">
      Enter Your Full Name :
      <input name="Fullname" type="text" value="<?php echo $_POST['Fullname']; ?>"><br/><br/>
      Enter Your Address :
      <textarea name="UserAddress" rows="3" cols="20"><?php echo $_POST['UserAddress']; ?></textarea>
      <br/><br/>
      <input name="BtnSubmit" type="submit" value="Submit">
</form>
