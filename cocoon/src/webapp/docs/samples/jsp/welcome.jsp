<%@ page import="java.util.*" %>
<%
	response.setHeader("Expires", "0");
%>

<html>

<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<title>JSPreader test</title>
<style type="text/css">
BODY {background-color: #FFFFFF; color: #660000; font-family: Verdana, Helvetica, Arial; }
</style>
</head>

<body>
<h3>This is a dynamic output from the JSPReader</h3>
<h4>Current time: <%=new Date()%></h4>
<hr noshade size="1">
<small><a href="..">Back to samples</a></small>
</body>
</html>
