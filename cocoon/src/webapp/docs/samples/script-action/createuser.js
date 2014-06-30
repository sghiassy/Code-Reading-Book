// Step 1 -- Retrieve helper "beans" from the BSF framework

scriptaction = bsf.lookupBean( "scriptaction" )
manager      = bsf.lookupBean( "manager" )            
request      = bsf.lookupBean( "request" )
logger       = bsf.lookupBean( "logger" )
actionMap    = bsf.lookupBean( "actionMap" )

// Step 2 -- Perform the action

logger.debug( "START createuser.js" )

// Retrieve things from the session and request
// NOTE: they are all of type java.lang.String

session = request.getSession( false )
uwid 	= session.getAttribute( "uwid" )

logger.debug( "Raw" )
logger.debug( "  uwid [" + uwid + "]" )

// We have the choice of declaring things out here and making them explicitly
// null, or we have to use a different comparison in the "finally" block (defined?)

dbselector = null
datasource = null
conn = null
queryStatement = null
insertStatement = null

try
{
    dbselector = manager.lookup( scriptaction.DB_CONNECTION )
    datasource = dbselector.select( "ceabplanner" )
    conn = datasource.getConnection()    

    // Check that the student exists

    queryStatement = conn.prepareStatement(
              "select count(*) here from students where uw_userid=? "
    )
              
    queryStatement.setString( 1, uwid )
      
    resultSet = queryStatement.executeQuery()
    resultSet.next()
	 
    userExists = resultSet.getInt("here")
    
    logger.debug( "Result #1 [" + userExists + "]" )

    if (!userExists)
    {

        logger.debug( "User does not exist...creating and initializing" )

        insertStatement = conn.prepareStatement(
                 "INSERT INTO students (id, uw_userid, name, uw_id, current_term) values (students_seq.nextval, ?, '','', (SELECT id FROM terms WHERE description = '1A'))"
        )

        insertStatement.setString( 1, uwid ); 

        result = insertStatement.executeUpdate()
        logger.debug( "Result #2 [" + result + "]" )

        insertStatement = conn.prepareStatement(
                 "insert into studentNotes (student,note) values ( ( SELECT id FROM students WHERE uw_userid = ? ),'')"
        )

        insertStatement.setString( 1, uwid ); 

        result = insertStatement.executeUpdate()
        logger.debug( "Result #3 [" + result + "]" )

        insertStatement = conn.prepareStatement(
                 "INSERT INTO studentKnownCourseList (student, known_course, term_taken, distance_ed, course_extra) ( SELECT ( SELECT id FROM students WHERE uw_userid = ? ), course, term, '1', '1' FROM corecourses )"
        )

        insertStatement.setString( 1, uwid ); 

        result = insertStatement.executeUpdate()
        logger.debug( "Result #4 [" + result + "]" )

        conn.commit()
    }
    else
    {
        logger.debug( "User exists" )
    }
    actionMap.put( "scriptaction-continue", "" )
}
catch( ex )
{
    logger.debug( "Caught Exception" )
    logger.debug( "  " + ex )
}
finally
{
    if ( null != queryStatement ) { queryStatement.close() }
    if ( null != insertStatement ) { insertStatement.close() }
    if ( null != conn ) { conn.close() }
    if ( null != datasource ) { dbselector.release( datasource ) }
    if ( null != dbselector ) { manager.release( dbselector ) }
}

logger.debug( "END createuser.js" )
