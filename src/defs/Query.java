/******************
  Copyright (c) 2002                                        RIPE NCC

  All Rights Reserved

  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name of the author not be
  used in advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
  AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  ***************************************/

/**
 * RIPE attribute.
 *
 * @author ottrey@ripe.net
 * @version $Version$
 *
 */
public class Query {
  
  private String qrytype;
  private boolean lookup;
  private String keytype;
  private String code;
  private String clars;
  private String sqlQuery;
  private String Querydesc;

  // -----------------oOo-----------------
  //              Constructors
  // -----------------oOo-----------------
  /**
   * Creates a Query structure.
   *               
   * @author ottrey@ripe.net
   *               
   */
  public Query(String qrytype, boolean lookup, String keytype, String code, String clars, String sqlQuery) {
   
    this.qrytype = qrytype;
    this.lookup = lookup;
    this.keytype = keytype.toUpperCase();
    this.code = code;
    this.clars = clars;
    this.sqlQuery = sqlQuery;

    // now prepare a query description 
    if( qrytype.equals("SQL") ) {
	Querydesc = qrytype + ":" + clars + "/" + code + "/" + this.keytype;
    } else {
	Querydesc = qrytype + ":" + clars + "/" + this.keytype;
    }
  }// Query()

    private String replaceVars(Defs defs) {
	String finalQuery = "";
	int lastindex = 0;
	
	
	// paste into finalQuery, replacing the symbols
	do {
	    int begin, end;
	    
	    begin = sqlQuery.indexOf("$(", lastindex);
	    
	    if( begin != -1 ) {
		end = sqlQuery.indexOf(")", begin);
		String variable = sqlQuery.substring(begin+2, end);
		
		//System.out.println( "var: " +  variable );
		
		finalQuery = finalQuery.concat(sqlQuery.substring(lastindex, begin));
		finalQuery = finalQuery.concat(defs.getValueByEnum(variable) );
		lastindex = end + 1;
	    } else {
		//System.out.println( "finishing: " + sqlQuery.substring(lastindex));
		finalQuery = finalQuery.concat(sqlQuery.substring(lastindex));
		lastindex = sqlQuery.length();
	    }
	    
	    //System.out.println( "final: " + finalQuery);
	    
	} while( lastindex < sqlQuery.length() );
	
	return finalQuery;
	
    } // replaceVars

  /**
   * @return String C - structure in the form:
   * {
   *   R_SQL,
   *   Q_LOOKUP,
   *   WK_NAME,
   *   A_PN,
   *   "SELECT N01.pe_ro_id FROM %s WHERE %s",
   *   0, 
   *   0  
   * },
   * @param String the level of indenting.
   *
   */
  public String getStruct(String indent, Defs defs) {
    String result = new String();
    
    String FinalQuery = replaceVars(defs);

    result += indent + "{" + "\n  ";
    result += indent + "R_" + qrytype.toUpperCase() + ",\n  ";
    if (lookup) {
      result += indent + "Q_LOOKUP";
    }                                       
    else {                                  
      result += indent + "Q_INVERSE";
    }                                       
    result += ",\n  ";
    result += indent + keytype.toUpperCase() + ",\n  ";
    result += indent + "A_" + code.toUpperCase() + ",\n  ";
    if (clars.equals("ANY")) {
      result += indent + "C_ANY" + ",\n  ";
    }
    else {
      result += indent + "C_" + clars.toUpperCase() + ",\n  ";
    }
    result += indent + "\"" + FinalQuery.replace('\n', ' ') + "\""  + ",\n  ";

    result += indent + "\"" + Querydesc  + "\""  + "\n  ";

    result += indent + "},";

    return result;
  } //  getStruct()
 
  public static String startDoc() {
    String result = new String();

    // depend on external structure definition in defs.h
    result += "Query_t Query[] = {\n";

    return result;
  } // startDoc()

  public static String endDoc() {
    String result = new String();

    result += "  {\n";
    result += "    0,\n";
    result += "    0,\n";
    result += "    0,\n";
    result += "    0,\n";
    result += "    0,\n";
    result += "    NULL\n";
    result += "  }\n";
    result += "}; /* Query[] */\n";

    return result;
  } // endDoc()

} // Query

