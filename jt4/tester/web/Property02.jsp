<html>
<head>
<title>Property02.jsp - Negative PropertyEditor Test</title>
</head>
<body bgcolor="white">
<jsp:useBean id="bean" scope="request"
          class="org.apache.tester.SessionBean"/>
<jsp:setProperty name="bean" property="dateProperty"
                value="07/25/200A"/>
Date property is '<jsp:getProperty name="bean" property="dateProperty"/>'.
</body>
</html>
