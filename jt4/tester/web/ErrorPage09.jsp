<%@ page contentType="text/plain" errorPage="/ErrorPage10.jsp" %><%

        // Write a FAILED message that should get replaced by the error text
        out.println("ErrorPage09 FAILED - Original response returned");

        // Throw the specified exception
        int i = 1;
        if (i > 0) {
            throw new ArrayIndexOutOfBoundsException
                ("ErrorPage09 Threw ArrayIndexOutOfBoundsException");
        }

%>
