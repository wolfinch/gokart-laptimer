<html>
<style type="text/css">
.tg-legend {
	width: 15%;
	border-collapse: collapse;
	border-spacing: 0;
	border-color: #aabcfe;
	margin: 0px 0px;
	align: left;
	text-align: left;
	background-color: #e8edff;	
}

.tg-legend td {
	vertical-align: middle;
	padding: 5px 10px;
	font-family: Arial, sans-serif;
	font-weight: bold;
	font-size: 15px;
	overflow: hidden;
	color: #039;
	border-width: 1px;	
	border-style: solid;
}

.tg-legend .ir{
	width: 60%;
	border-collapse: collapse;
	border-spacing: 0;
	border-color: #aabcfe;
	margin: 0px 0px;
	align: left;
	text-align: left;
	background-color: #039;	
}
.tg-legend .hall{
	width: 60%;
	border-collapse: collapse;
	border-spacing: 0;
	border-color: #aabcfe;
	margin: 0px 0px;
	align: left;
	text-align: left;
	background-color: #E89619;	
}
</style>

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
			$('#score_card').load('score_card.php');
		});
</script>

<body>
	<div id="score_card" align="center"></div>
	<br>
	<table class="tg-legend">
	<tr>
	<td colspan="2" align="center">
	Legend
	</td>
	</tr>
	<tr>
	<td width="50%" class="ir"> </td>
	<td> IR </td> 
	</tr>
	<tr>
	<td width="50%" class="hall"> </td>
	<td> Magnetic </td>
	</tr>
	</table>
</body>
<footer align='Center' > 
© Copyright 2015 Joshith (joe.cet@gmail.com), all rights reserved 
</footer>
</html>
