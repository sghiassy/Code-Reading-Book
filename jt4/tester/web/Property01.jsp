<html>
<head>
<title>Property01.jsp - Positive PropertyEditor Test</title>
</head>
<body bgcolor="white">
<jsp:useBean id="bean" scope="request"
          class="org.apache.tester.SessionBean"/>
<jsp:setProperty name="bean" property="dateProperty"
                value="07/25/2001"/>
Date property is '<jsp:getProperty name="bean" property="dateProperty"/>'.
</body>
</html>
