// Step 1 -- Retrieve helper "beans" from the BSF framework

scriptaction = bsf.lookupBean( "scriptaction" )
manager      = bsf.lookupBean( "manager" )            
request      = bsf.lookupBean( "request" )
logger       = bsf.lookupBean( "logger" )
actionMap    = bsf.lookupBean( "actionMap" )

// Step 2 -- Perform the action

logger.debug( "START addknowncourse.js" )

// Retrieve things from the session and request
// NOTE: they are all of type java.lang.String

session 	= request.getSession( false )
uwid 		= session.getAttribute( "uwid" )
term 		= request.getParameter( "term" )
course 		= request.getParameter( "course" )
distanceEd 	= request.getParameter( "distanceEd" )
extra 		= request.getParameter( "extra" )

logger.debug( "Raw" )
logger.debug( "  uwid          [" + uwid + "]" )
logger.debug( "  term          [" + term + "]" )
logger.debug( "  course        [" + course + "]" )
logger.debug( "  distanceEd    [" + distanceEd + "]" )
logger.debug( "  extra         [" + extra + "]" )

// Cook things a little to make them palatable to the database

if ( term == "0" ) { term = "" }
if ( course == "0" ) { course = "" }
if ( distanceEd == "on" ) { distanceEd = 2 } else { distanceEd = 1 }
if ( extra == "on" ) { extra = 2 } else { extra = 1 }

logger.debug( "Cooked" )
logger.debug( "  uwid          [" + uwid + "]" )
logger.debug( "  term          [" + term + "]" )
logger.debug( "  course        [" + course + "]" )
logger.debug( "  distanceEd    [" + distanceEd + "]" )
logger.debug( "  extra         [" + extra + "]" )

// Actually do the database work

// We have the choice of declaring things out here and making them explicitly
// null, or we have to use a different comparison in the "finally" block (defined?)

dbselector = null
datasource = null
conn = null
addStatement = null

try
{
    dbselector = manager.lookup( scriptaction.DB_CONNECTION )
    datasource = dbselector.select( "ceabplanner" )
    conn = datasource.getConnection()    
    addStatement = conn.prepareStatement(
    "INSERT INTO studentKnownCourseList ( STUDENT, KNOWN_COURSE, TERM_TAKEN, DISTANCE_ED, COURSE_EXTRA)" +
    "VALUES  ( ( SELECT id FROM students WHERE uw_userid = ? ), ?, ?, ?, ? )"
    )
    
    addStatement.setString( 1, uwid ); 
    addStatement.setString( 2, course ); 
    addStatement.setString( 3, term ); 
    addStatement.setString( 4, distanceEd ); 
    addStatement.setString( 5, extra ); 
    
    result = addStatement.executeUpdate()
    logger.debug( "Result #1 [" + result + "]" )
    
    conn.commit()
    
    actionMap.put( "scriptaction-continue", "" )
    session.setAttribute( "results", "<SUCCESS>Course added at " + Date() + "</SUCCESS>" )    
}
catch( ex )
{
    logger.debug( "Caught Exception" )
    logger.debug( "  " + ex )
}
finally
{
    if ( null != addStatement ) { addStatement.close() }
    if ( null != conn ) { conn.close() }
    if ( null != datasource ) { dbselector.release( datasource ) }
    if ( null != dbselector ) { manager.release( dbselector ) }
}

logger.debug( "END addknown.js" )
