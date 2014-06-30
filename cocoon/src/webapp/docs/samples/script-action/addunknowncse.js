// Step 1 -- Retrieve helper "beans" from the BSF framework

scriptaction = bsf.lookupBean( "scriptaction" )
manager      = bsf.lookupBean( "manager" )            
request      = bsf.lookupBean( "request" )
logger       = bsf.lookupBean( "logger" )
actionMap    = bsf.lookupBean( "actionMap" )

// Step 2 -- Perform the action

logger.debug( "START addunknown.js" )

lectureHrs = 3
labHrs = 0
tutorialHrs = 0
numWeeks = 12
courseWeight = 1
cseweight = 0
math = 0
sci = 0
engsci = 0
engdes = 0

// Retrieve things from the session and request
// NOTE: they are all of type java.lang.String

session    = request.getSession( false )
uwid       = session.getAttribute( "uwid" )
term       = request.getParameter( "term" )
dept       = request.getParameter( "dept" )
number     = request.getParameter( "number" )
letter     = request.getParameter( "letter" )
name       = request.getParameter( "name" )
lectures   = request.getParameter( "lectures" )
labs       = request.getParameter( "labs" )
tutorials  = request.getParameter( "tutorials" )
weeks      = request.getParameter( "weeks" )
csetype    = request.getParameter( "csetype" )
weight     = request.getParameter( "weight" )
distanceEd = request.getParameter( "distanceEd" )
extra      = request.getParameter( "extra" )

logger.debug( "Raw" )
logger.debug( "  uwid          [" + uwid + "]" )
logger.debug( "  term          [" + term + "]" )
logger.debug( "  dept          [" + dept + "]" )
logger.debug( "  number        [" + number + "]" )
logger.debug( "  letter        [" + letter + "]" )
logger.debug( "  name          [" + name + "]" )
logger.debug( "  lectures      [" + lectures + "]" )
logger.debug( "  labs          [" + labs + "]" )
logger.debug( "  tutorials     [" + tutorials + "]" )
logger.debug( "  weeks         [" + weeks + "]" )
logger.debug( "  math          [" + math + "]" )
logger.debug( "  sci           [" + sci + "]" )
logger.debug( "  engsci        [" + engsci + "]" )
logger.debug( "  engdes        [" + engdes + "]" )
logger.debug( "  csetype       [" + csetype + "]" )
logger.debug( "  weight        [" + weight + "]" )
logger.debug( "  distanceEd    [" + distanceEd + "]" )
logger.debug( "  extra         [" + extra + "]" )
logger.debug( "  lectureHrs    [" + lectureHrs + "]" )
logger.debug( "  labHrs        [" + labHrs + "]" )
logger.debug( "  tutorialHrs   [" + tutorialHrs + "]" )
logger.debug( "  numWeeks      [" + numWeeks + "]" )
logger.debug( "  courseWeight  [" + courseWeight + "]" )
logger.debug( "  cseweight     [" + cseweight + "]" )

// Cook things a little to make them palatable to the database

if (lectures != null) { lectureHrs = ( lectures + "" ) }
if (labs != null) { labHrs = ( labs + "" ) }
if (tutorials != null) { tutorialHrs = ( tutorials + "" ) }
if (weeks != null) { numWeeks = ( weeks + "" ) }
if (weight != null) { courseWeight = ( weight + "" ) }

// The "/1" is vital to ensure a numeric context

cseweight=(((lectureHrs/1)+(labHrs/2)+(tutorialHrs/2))*numWeeks*courseWeight)

if ( distanceEd == "on" ) { distanceEd = 2 } else { distanceEd = 1 }
if ( extra == "on" ) { extra = 2 } else { extra = 1 }

logger.debug( "Cooked" )
logger.debug( "  uwid          [" + uwid + "]" )
logger.debug( "  term          [" + term + "]" )
logger.debug( "  dept          [" + dept + "]" )
logger.debug( "  number        [" + number + "]" )
logger.debug( "  letter        [" + letter + "]" )
logger.debug( "  name          [" + name + "]" )
logger.debug( "  lectures      [" + lectures + "]" )
logger.debug( "  labs          [" + labs + "]" )
logger.debug( "  tutorials     [" + tutorials + "]" )
logger.debug( "  weeks         [" + weeks + "]" )
logger.debug( "  math          [" + math + "]" )
logger.debug( "  sci           [" + sci + "]" )
logger.debug( "  engsci        [" + engsci + "]" )
logger.debug( "  engdes        [" + engdes + "]" )
logger.debug( "  csetype       [" + csetype + "]" )
logger.debug( "  weight        [" + weight + "]" )
logger.debug( "  distanceEd    [" + distanceEd + "]" )
logger.debug( "  extra         [" + extra + "]" )
logger.debug( "  lectureHrs    [" + lectureHrs + "]" )
logger.debug( "  labHrs        [" + labHrs + "]" )
logger.debug( "  tutorialHrs   [" + tutorialHrs + "]" )
logger.debug( "  numWeeks      [" + numWeeks + "]" )
logger.debug( "  courseWeight  [" + courseWeight + "]" )
logger.debug( "  cseweight     [" + cseweight + "]" )

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
       "INSERT INTO otherCourses( ID, STUDENT, DEPARTMENT, COURSENUMBER, COURSELETTER, COURSENAME, MATH, SCI, ENGSCI, ENGDES, CSETYPE, CSEWEIGHT, TERM_TAKEN, DISTANCE_ED, COURSE_EXTRA) VALUES  ( otherCourses_seq.nextval, ( SELECT id FROM students WHERE uw_userid = ? ), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )"
    )

    addStatement.setString( 1, uwid ); 
    addStatement.setString( 2, dept ); 
    addStatement.setString( 3, number ); 
    addStatement.setString( 4, letter ); 
    addStatement.setString( 5, name ); 
    addStatement.setString( 6, math ); 
    addStatement.setString( 7, sci ); 
    addStatement.setString( 8, engsci ); 
    addStatement.setString( 9, engdes ); 
    addStatement.setString( 10, csetype ); 
    addStatement.setString( 11, cseweight ); 
    addStatement.setString( 12, term ); 
    addStatement.setString( 13, distanceEd ); 
    addStatement.setString( 14, extra ); 

    result = addStatement.executeUpdate()
    logger.debug( "Result #1 [" + result + "]" )

    conn.commit()

    actionMap.put( "scriptaction-continue", "" )
    session.setAttribute( "results", "<SUCCESS>Unknown Complementary Studies course added at " + Date() + "</SUCCESS>" )
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


logger.debug( "END addunknown.js" )
