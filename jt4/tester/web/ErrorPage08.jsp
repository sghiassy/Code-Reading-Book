<%@ page contentType="text/plain" %><%

        // Write a FAILED message that should get replaced by the error text
        out.println("ErrorPage08 FAILED - Original response returned");

        // Throw the specified exception
        String type = request.getParameter("type");
        if ("Arithmetic".equals(type)) {
            throw new ArithmeticException
                ("ErrorPage08 Threw ArithmeticException");
        } else if ("Array".equals(type)) {
            throw new ArrayIndexOutOfBoundsException
                ("ErrorPage08 Threw ArrayIndexOutOfBoundsException");
        } else if ("Number".equals(type)) {
            throw new NumberFormatException
                ("ErrorPage08 Threw NumberFormatException");
        }

%>
