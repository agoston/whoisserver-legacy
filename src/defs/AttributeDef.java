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

import java.util.*;
import org.w3c.dom.*;
import org.xml.sax.*;

/**
 * RIPE attribute.
 *
 * @author ottrey@ripe.net
 * @version $Version$
 *
 */
public class AttributeDef implements Cloneable {
  
  final static int QI_SQL   = 1;
  final static int QI_RADIX = 2;

  private String name;
  private String altName;
  private String xmlName;
  private String code;
  private String status;

  private String description;
  private String syntax;

  private boolean lookup;
  private boolean inverse;
  private boolean primary;
  private boolean list;
  private boolean ripeList;
  private String foreign;
  private String keytype;

    // UD loading/updating/deleting queries
  private String insert;
  private String insertQ_type;
  private String update;
  private String updateQ_type;
  private String dummy;
  private String dummyQ_type;
  private String select;
  private String selectQ_type;

  // these actually come from the RPSL *class* definition, but they got
  // stuck in this Java class anyway...
  private String choice;
  private String number;
  private boolean pseudo;    // a pseudo-attribute is "delete" or "override"
                             // these are not "real" attributes, and as such
                             // should not appear in template definitions
    
  // radix tree representation
  private String family;
  private String load_ipv4; // query to load the ipv4 tree
  private String load_ipv6; // query to load the ipv6 tree

  private Vector queries;

  // -----------------oOo-----------------
  //              Constructors
  // -----------------oOo-----------------
  /**
   * Creates a RIPE attribute.
   *               
   * @author ottrey@ripe.net
   * @version $Version$
   *               
   * @param obj The node from which a RIPE attribute is made.
   * 
   */
  public AttributeDef(Node obj) {
    name      = obj.getAttributes().getNamedItem("name").getNodeValue();
    code      = obj.getAttributes().getNamedItem("code").getNodeValue();
    status    = obj.getAttributes().getNamedItem("status").getNodeValue();

    // Blindly ask for the optional items.
    try {
      altName   = obj.getAttributes().getNamedItem("altName").getNodeValue();
    }
    catch (NullPointerException e) {
      altName = new String();
      // Throw the exception away.  :-)
    }

    // get xml name if defined, otherwise set to name
    try 
    {
        xmlName = obj.getAttributes().getNamedItem("xmlname").getNodeValue();
    } 
    catch (NullPointerException e) 
    {
        xmlName = name;
    }

    // Initialize
    foreign = new String();
    lookup = false;
    inverse = false;
    primary = false;
    list = false;
    ripeList = false;

    insert       = new String();
    insertQ_type = new String("UD_NULL_");
    update       = new String();
    updateQ_type = new String("UD_NULL_");
    dummy        = new String();
    dummyQ_type  = new String("UD_NULL_");
    select       = new String();
    selectQ_type = new String("UD_NULL_");
    
    queries = new Vector();

    // representation Node
    Node rp = null;

    // keys node
    Node kn = null;

    // loop to retrieve elements
    Node search = obj.getFirstChild();
    while (search != null) {
        String nodeName = search.getNodeName();
        if (nodeName.equals("description")) {
            // Get the "description" node.
            description = getNodeRawValue(search);
        } else if (nodeName.equals("syntax")) {
            Node listSearch = search.getFirstChild();
            while (listSearch != null) {
                if (listSearch.getNodeName().equals("list")) {
                    list = true;
                } else if (listSearch.getNodeName().equals("ripe-list")) {
                    ripeList = true;
                }
                listSearch = listSearch.getNextSibling();
            }
            // Get the "syntax" node.
            syntax = getNodeRawValue(search);
        } else if (nodeName.equals("representation")) {
            rp = search;
        } else if (nodeName.equals("keys")) {
            kn = search;
        }
        search = search.getNextSibling();
    }

    // find representation details
    Node sqlTable = null;
    if (rp != null) {
        Node rpSearch = rp.getFirstChild();
        while (rpSearch != null) {
            if (rpSearch.getNodeName().equals("sqltable")) {
                sqlTable = rpSearch;
            } else if (rpSearch.getNodeName().equals("radixtrees")) {
                NamedNodeMap map = rpSearch.getAttributes();
                family = map.getNamedItem("family").getNodeValue();
                Node rxSearch = rpSearch.getFirstChild();
                while (rxSearch != null) {
                    if (rxSearch.getNodeName().equals("IP_V4")) {
	                load_ipv4 = getTextFromNode(rxSearch);
                    } else if (rxSearch.getNodeName().equals("IP_V6")) {
                        load_ipv6 = getTextFromNode(rxSearch);
                    }
                    rxSearch = rxSearch.getNextSibling();
                }
            }
            rpSearch = rpSearch.getNextSibling();
        }
    }
    // get sqltable details here
    if (sqlTable != null) {
        Node sqlTableSearch = sqlTable.getFirstChild();
        while (sqlTableSearch != null) {
            String nodeName = sqlTableSearch.getNodeName();
            if (nodeName.equals("insert")) {
                // Get the insert.
                insert = getTextFromNode(sqlTableSearch);
 	        if (insert.length() > 0) {
	            insert = " " + insert + " ";
	        }
                Node qtypeAttr = 
                  sqlTableSearch.getAttributes().getNamedItem("qtype");
                if (qtypeAttr != null) {
                    insertQ_type = qtypeAttr.getNodeValue().toUpperCase();
                } 
            } else if (nodeName.equals("update")) {
                // Get the updates.
                update = getTextFromNode(sqlTableSearch);
	        if (update.length() > 0) {
	            update = " " + update + " ";
	        }
                Node qtypeAttr = 
                  sqlTableSearch.getAttributes().getNamedItem("qtype");
                if (qtypeAttr != null) {
                    updateQ_type = qtypeAttr.getNodeValue().toUpperCase();
                }
            } else if (nodeName.equals("dummy")) {
                // Get the dummies.
                dummy = getTextFromNode(sqlTableSearch);
	        if (dummy.length() > 0) {
	            dummy = " " + dummy + " ";
	        }
                Node qtypeAttr = 
                  sqlTableSearch.getAttributes().getNamedItem("qtype");
                if (qtypeAttr != null) {
                    dummyQ_type = qtypeAttr.getNodeValue().toUpperCase();
                }
            } else if (nodeName.equals("select")) {
                // Get the selects.
                select = getTextFromNode(sqlTableSearch);
	        if (select.length() > 0) {
	            select = " " + select + " ";
	        }
                Node qtypeAttr = 
                  sqlTableSearch.getAttributes().getNamedItem("qtype");
                if (qtypeAttr != null) {
                    selectQ_type = qtypeAttr.getNodeValue().toUpperCase();
                }
            } 
            sqlTableSearch = sqlTableSearch.getNextSibling();
        }
    } // rp!=NULL

    if (kn != null) {
      String searchable = 
        kn.getAttributes().getNamedItem("searchable").getNodeValue();
      inverse = searchable.equals("inverse");
      lookup = searchable.equals("lookup");

      Node knSearch = kn.getFirstChild();
      while (knSearch != null) {
          if (knSearch.getNodeName().equals("foreign")) {
              Node value = knSearch.getAttributes().getNamedItem("value");
              foreign = value.getNodeValue();
          } else if (knSearch.getNodeName().equals("primary")) { 
              primary = true;
          } else if (knSearch.getNodeName().equals("queries")) {
              // Get the queries.
              appendQueries(queries, knSearch, "sqlquery", code);
              appendQueries(queries, knSearch, "radixquery", code);
          }
          knSearch = knSearch.getNextSibling();
      }
    }

    choice = "";
    number = "";
    pseudo = false;

    // Now check cominations.
    // XXX TODO

  } // AttributeDef()

  private void appendQueries(Vector queries, Node qsn, String qrytype, String attrcode) {
    // if we have no node, exit
    if (qsn == null) {
        return;
    } 

    // setup based on our query type (or exit if unknown)
    String qryt;
    if (qrytype.equals("sqlquery")) {
        qryt = "SQL";
    } else {
        qryt = "RADIX";
    }

    Node q = qsn.getFirstChild();
    while (q != null) {

        if (q.getNodeName().equals(qrytype)) {
            String keytype = 
              q.getAttributes().getNamedItem("keytype").getNodeValue();

            // Blindly get the optional values.
            String clars = new String();
            try {
              clars = q.getAttributes().getNamedItem("class").getNodeValue();
            }
            catch (NullPointerException e) {
    	      // XXX take the default
    	      clars = attrcode;
            }

            String space = new String();
            try {
	        space = q.getAttributes().getNamedItem("space").getNodeValue();
	    }
            catch (NullPointerException e) {
            }


	    String sqlQuery = getTextFromNode(q);
	    //System.err.println("sqlquery = " + sqlQuery);

            Query query = new Query(qryt, lookup, keytype, code, 
                                    clars, sqlQuery);
            queries.addElement(query);
        }

        q = q.getNextSibling();
    }
  } // getQueries()


    
    // getting parsed contents of the text node is not simple.
    // see http://www.developerlife.com/xmljavatutorial1/default.htm
    
    // it was made simpler by the getNodeValue(Node n) method 
    // defined below, but it operated on raw XML text fragments
    
/* deleted, expunged, and otherwise removed - Shane 
private String getTextFromNode( Node q ) {
    Element query_elem = (Element) q;
    NodeList list = query_elem.getChildNodes();
    int size = list.getLength();
    
    for (int i = 0 ; i < size ; i ++ ){
        // skip non-text nodes (e.g. comments)
        if (list.item(i).getNodeType() != Node.TEXT_NODE) {
            continue;
        }
	String value =
	    ((Node)list.item( i )).getNodeValue().trim();
	//System.err.println("i=" + i + " val=" + value );
	
	if( value.equals("") || value.equals("\r") ){
	    continue; //keep iterating
	}
	else{
	    return value;
	}
    }
    
    return "";
  }
*/

  // shorter, friendlier version (Shane)
  private String getTextFromNode(Node node)
  {
      // concatenate all text nodes together
      StringBuffer sb = new StringBuffer();
      Node kid = node.getFirstChild();
      while (kid != null) {
          if (kid.getNodeType() == Node.TEXT_NODE) {
              sb.append(kid.getNodeValue());
          }
          kid = kid.getNextSibling();
      }
      // convert newlines to spaces
      for (int i=0; i<sb.length(); i++) {
          char c = sb.charAt(i);
          if ((c == '\r') || (c == '\n')) {
              sb.setCharAt(i, ' ');
          }
      }
      return sb.toString().trim();
  }
  
  /**
   * Aaaargh I shouldn't have to write this. :-(
   *
   * @param        node
   * @return       The value of the node.
   * @see          ClassDef
   *
   */
  private String getNodeRawValue(Node node) {
      StringBuffer sb = new StringBuffer();
      Node kid = node.getFirstChild();
      while (kid != null) {
          if (kid.getNodeType() == Node.TEXT_NODE) {
              sb.append(kid.getNodeValue());
          }
          kid = kid.getNextSibling();
      }
      return sb.toString();
  } // getNodeRawValue()
  
  public String getFamily() {
    return family;
  } 

  public String getV4Load() {
    return load_ipv4;
  } 

  public String getV6Load() {
    return load_ipv6;
  } 

  public String getCode() {
    return code;
  } // getCode()

  public String getName() {
    return name;
  } // getName()

  public String getAltName() {
    return altName;
  } // getAltName()

  public String getXmlName() {
    return xmlName;
  } // getAltName()

  public String getStatus() {
    return status;
  } // getStatus()

  public String getDescription() {
    return description;
  } // getDescription()

  public String getSyntax() {
    return syntax;
  } // getSyntax()

  public String getEnumeration() {
    return new String("A_" + code).toUpperCase();
  } // getEnum()

  public String getChoice() {
    return choice;
  } // getChoice()

  public String getNumber() {
    return number;
  } // getNumber()

  public boolean getPseudo() 
  {
      return pseudo;
  }

  public String getKeytype() {
    return keytype;
  } // getKeytype()

  public String getInsert() {
    return insert;
  } // getInsert()

  public String getInsertQ_type() {
    return insertQ_type;
  } // getInsertQ_type()

  public String getUpdate() {
    return update;
  } // getUpdate()

  public String getUpdateQ_type() {
    return updateQ_type;
  } // getUpdateQ_type()

  public String getDummy() {
    return dummy;
  } // getDummy()

  public String getDummyQ_type() {
    return dummyQ_type;
  } // getDummyQ_type()

  public String getSelect() {
    return select;
  } // getSelect()

  public String getSelectQ_type() {
    return selectQ_type;
  } // getSelectQ_type()

  public String getKeytype2() {
    String result = new String();

    if      (!lookup && !inverse && !primary) {
      result = " ";
    }
    else if (!lookup && !inverse &&  primary) {
      result = "primary key";
    }
    else if (!lookup &&  inverse && !primary) {
      result = "inverse key";
    }
    else if (!lookup &&  inverse &&  primary) {
      result = "primary/inverse key";
    }
    else if ( lookup && !inverse && !primary) {
      result = "lookup key";
    }
    else if ( lookup && !inverse &&  primary) {
      result = "primary/look-up key";
    }
    else if ( lookup &&  inverse && !primary) {
      result = "look-up/inverse key";
    }
    else if ( lookup &&  inverse &&  primary) {
      result = "Gimmie a break!";
    }

    return result;
  } // getKeytype()

  public String getKeytype3() {
    String result = new String();
    
    if (primary) {
      result = "[P]";
    }
    else  {
      result = "   ";
    }

    if (inverse) {
      result += "[I]";
    }
    else if (lookup) {
      result += "[L]";
    }
    else {
      result += "   ";
    }

    return result;
  } // getKeytype()

  public String getForeign() {
    return foreign;
  } // getForeign()

  public boolean getLookup() {
      return lookup;
  }

  public boolean getInverse() {
    return inverse;
  } // getInverse()

  public boolean getPrimary() {
    return primary;
  } // getPrimary()

  public boolean getList() {
      return list;
  }

  public boolean getRipeList() {
      return ripeList;
  }

  public Vector getQueries() {
    return queries;
  } // getQueries()

  public boolean setChoice(String choice) {
    boolean result=true;

    this.choice = choice;

    return result;
  } // setChoice()

  public boolean setNumber(String number) {
    boolean result=true;

    this.number = number;

    return result;
  } // setNumber()

  public void setPseudo(boolean pseudo)
  {
      this.pseudo = pseudo;
  }

  public Object clone() throws CloneNotSupportedException {
    return (AttributeDef)super.clone();
  } // clone()

  /*
  public boolean equals(String code) {
    return code.equals(code);
  } // equals()
  */
  
  public String toString() {
    return new String("ripe attribute={" +
                         "\n\tname="        + name        +
                         "\n\taltName="     + altName     +
                         "\n\txmlName="     + xmlName     +
                         "\n\tcode="        + code        +
                         "\n\tstatus="      + status      +
                         "\n\tkeytype="     + keytype     +
                         "\n\tdescription=" + description +
                         "\n\tsyntax="      + syntax      +
                         "\n\tchoice="      + choice      +
                         "\n\tnumber="      + number      +
                         "\n}");
  } // toString()


} // AttributeDef
