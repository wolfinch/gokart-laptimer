<html>
<head>
<title>Gokart Session</title>
<style type="text/css">
body {
	background-image: url("f1-bg.jpg");
	background-repeat: no-repeat;
	background-size: cover;
}
</style>
<script type="text/javascript"
	 src="jquery-1.11.2.min.js"> </script>
</head>

<script>

var auto_refresh = setInterval(
		function()
		{
			$('#score_card').load('score_card.php');
		}, 5000);
		$(document).ready(function(){
			$('#score_card').load('score_card.php')
		})
</script>

<body>
	<div id="score_card" align="center"></div>
</body>

</html>
