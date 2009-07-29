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

/**
 * RIPE class.
 *
 * @author ottrey@ripe.net
 * @version $Version$
 *
 */
public class ClassDef {

  public static final int EXTRA_BIT=11;

    public static final int DUMMIFY_NONE = 0;
    public static final int DUMMIFY_PLACEHOLDER = 1;
    public static final int DUMMIFY_FILTER = 2;

  private String    name;
  private String    enumeration;
  private String    code;
  private String    status;
  private String    main_table;
  private String    description;
  private Vector    attributes;
  private Vector    affected_tables;
  private Vector    rollback_tables;
  private Vector    delete_invquery_tables;
  private int       dbaseCode;
  private int       dummifyType;
  private String    dummifySingleton;

  private int       width;        // The longest attribute name.
  private Hashtable foreignAttrs;

  // -----------------oOo-----------------
  //              Constructors
  // -----------------oOo-----------------
  /**
   * Creates a RIPE class.
   *               
   * @author ottrey@ripe.net
   * @version $Version$
   *               
   * @param obj The node from which a RIPE class is made.
   * 
   * NOTE: No-one has implemented a Node.getNodeValue() method yet.  ;-(
   *       description = tw.getNextElement("description").getNodeValue();
   *       So I wrote a crappy one here.
   * @see getNodeValue
   */
  public ClassDef(Node obj, Hashtable ripeAttributes) {
    width = 0;
    name      = obj.getAttributes().getNamedItem("name").getNodeValue();
    code      = obj.getAttributes().getNamedItem("code").getNodeValue();
    status    = obj.getAttributes().getNamedItem("status").getNodeValue();
    main_table = obj.getAttributes().getNamedItem("main_sql_table").getNodeValue();
    enumeration      = new String("C_" + code).toUpperCase();

    Node search = obj.getFirstChild();
    while (search != null) {
        String nodeName = search.getNodeName();
        if (nodeName.equals("description")) {
            // Extract the "description".
            description = getNodeRawValue(search);
        } else if (nodeName.equals("attributes")) {
            // Create the attributes.
            attributes = createAttributes(search, ripeAttributes);
        } else if (nodeName.equals("affected-tables")) {
            affected_tables = getTableList(search);
        } else if (nodeName.equals("rollback-tables")) {
            rollback_tables = getTableList(search);
        } else if (nodeName.equals("delete-invquery-tables")) {
            delete_invquery_tables = getTableList(search);
        } else if (nodeName.equals("dummify")) {
            String dummifyTypeStr = search.getAttributes().getNamedItem("type").getNodeValue();
            if (dummifyTypeStr.equals("placeholder")) {
                dummifyType = DUMMIFY_PLACEHOLDER;
                dummifySingleton = search.getAttributes().getNamedItem("singleton").getNodeValue();
            } else if (dummifyTypeStr.equals("filter")) {
                dummifyType = DUMMIFY_FILTER;
            } else {
                throw new RuntimeException("Unknown dummification type '"+dummifyTypeStr+"'");
            }
        } else if (nodeName.equals("dbase_code")) {
            // Get the dbase_code.
            String dbaseCodeStr = 
              search.getAttributes().getNamedItem("value").getNodeValue();
            try {
              dbaseCode = Integer.parseInt(dbaseCodeStr);
            } catch (NumberFormatException e) {
              System.err.println("Bad dbaseCode: " + dbaseCodeStr); 
              System.err.println("\tencounterd in Object: " + obj); 
              System.exit(1);
            }
        } 
        search = search.getNextSibling();
    }

    // Create the foreignAttrs.
    foreignAttrs = createForeignAttrs();

  } // ClassDef()

  
  /**
   * Aaaargh I shouldn't have to write this. :-(
   *
   * @param        node
   * @return       The value of the node.
   * @see          ClassDef
   *
   */
  private String getNodeRawValue(Node node) {
//    String nodeStr = node.toString();
//    int startIndex = nodeStr.indexOf('>') + 1;
//    int endIndex = nodeStr.lastIndexOf('<') - 1;
//    
//    return nodeStr.substring(startIndex, endIndex);

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
  
  /**
   * Create the attributes for this class from the attributes already created.
   *
   * @param  attrs          The head node of the attributes tree from the classes DOM.
   * @param  ripeAttributes The attributes created from the attributes DOM.
   * @return       (methods only)
   *
   */
  private Vector createAttributes(Node attrs, Hashtable ripeAttributes) {
    // Create a set of attributes for this class.
    Vector result = new Vector();

    AttributeDef adTmp;
    String attrCode=null;
    String status;

    // Prepare to walk the tree.
//    try {
      Node attrNode = attrs.getFirstChild();
      // Get the attribute node from the tree.
      while (attrNode != null) {
        // skip random noise
        if (attrNode.getNodeType() != Node.ELEMENT_NODE) {
            attrNode = attrNode.getNextSibling();
            continue;
        }

        // Get the attribute code from the node.
        attrCode = new String(attrNode.getNodeName());

        // Get the attribute from the ripeAttributes.
        adTmp = (AttributeDef)ripeAttributes.get(attrCode);
        if (adTmp == null) 
        {
            System.err.println("Missing definition for attribute '" + attrCode + "'");
            System.exit(1);
        }

        // Keep track of the longest attribute name.
        if ( adTmp.getName().length() > width ) {
          width = adTmp.getName().length();
        }

        // If the attribute is "valid"
        if ((status=adTmp.getStatus()).equals("valid")) {

          // Create a clone of the attribute.
          AttributeDef ad = null;
          try {
            ad = (AttributeDef)adTmp.clone();
          }
          catch (CloneNotSupportedException e) {
            System.err.println("Doh!");
          }

          // Set the min attribute.
          ad.setChoice(attrNode.getAttributes().getNamedItem("choice").getNodeValue());

          // Set the max attribute.
          ad.setNumber(attrNode.getAttributes().getNamedItem("number").getNodeValue());

          // set the "pseudo" attribute, if requested
          Node pseudo = attrNode.getAttributes().getNamedItem("pseudo");
          if (pseudo != null) {
              if (pseudo.getNodeValue().equalsIgnoreCase("yes")) {
                  ad.setPseudo(true);
              }
          }

          // Add the attribute to this class.
          result.addElement(ad);
        }
        else {
          System.err.println("Warning: Class " + name + " contains a " +
                             status + " attribute: " + attrCode);
        }

        attrNode = attrNode.getNextSibling();
      }
//    }
//    catch (NullPointerException e) {
      // We tried to do something with a null - No worries, we caught it!
//      System.err.println("Trouble creating attributes: " + e);
//      System.err.println("attrCode=" + attrCode);
//      System.err.println("attrs=" + attrs);
//    }
    
    return result;
  } // createAttributes()

  private Vector getTableList(Node parent) {
    // Create a set of attributes for this class.
    Vector result = new Vector();

    // Prepare to walk the tree.
      Node attrNode = parent.getFirstChild();
      // Get the attribute node from the tree.
      while (attrNode != null) {
         // skip random noise
        if (attrNode.getNodeType() != Node.ELEMENT_NODE) {
            attrNode = attrNode.getNextSibling();
            continue;
        }

        result.addElement(attrNode.getAttributes().getNamedItem("name").getNodeValue());

        attrNode = attrNode.getNextSibling();
      }

      return result;

  }

  private Hashtable createForeignAttrs() {
    // Create a set of foreign attributes for this class.
    Hashtable result = new Hashtable();

    Enumeration e = attributes.elements();
    while (e.hasMoreElements()) {
      AttributeDef ad = (AttributeDef)e.nextElement();
//System.err.println("ad:" + ad);
      String code = ad.getCode();
      String foreign = ad.getForeign();
//System.err.println("foreign:" + foreign);
      if (foreign.length() > 1 ) {
        result.put(code, foreign);
      }
    }
    return result;
  } // createAttributes()

  private String toCString(String str) {
    String result = new String();
    char c;
    
    result += "\"";
    for(int i=0; i < str.length(); i++) {
      c = str.charAt(i);
      switch (c) {
        case '\n':
          result += "\\n\"\n\"";
        break;
        
        case '"':
          result += "\\\"";
        break;
        
        case '&':
          if(str.regionMatches(true, i, "&lt;", 0, 4)) {
            result += "<";
            i += 3;
          } else if(str.regionMatches(true, i, "&gt;", 0, 4)) {
            result += ">";
            i += 3;
          }
          else {
            result += c;
          }
        break;

        default:
          result += c;
      }
    }
    result += "\"";

    return result;
  } // toCString

  public String getName() {
    return name;
  } // getCode()

  public String getCode() {
    return code;
  } // getCode()

  public String getStatus() {
    return status;
  } // getStatus()

  public String getMainTable() {
    return main_table;
  } // getMainTable()

  public String getDescription(boolean CStyle) {
    String result = description;

    if (CStyle) {
      result = toCString(description);
    }

    return result;
  } // getDescription()

  public int getDbaseCode() {
    return dbaseCode;
  } // getDbaseCode()

  public int getWidth() {
    return width + EXTRA_BIT;
  } // getWidth()

  public Vector getAttributes() {
    return attributes;
  } // getAttributes()

  public Hashtable getForeignAttrs() {
    return foreignAttrs;
  } // getForeignAttrs()

  public String getEnumeration() {
    return enumeration;
  } // getEnum()

  public Vector getAffectedTables() {
      return affected_tables;
  } // getAffectedTables()

  public Vector getRollbackTables() {
      return rollback_tables;
  } // getRollbackTables()

  public Vector getInvQueryTables() {
      return delete_invquery_tables;
  } // getInvqueryTables()


 /**
  * Returns a template for the class in the form:
  *
  * boolean CStyle Returns in C style eg, with "\n\" at the end of each line.
  *
  * person:      [mandatory]  [single]     [primary/look-up key]    
  * address:     [mandatory]  [multiple]   [ ]                      
  * phone:       [mandatory]  [multiple]   [ ]                      
  * fax-no:      [optional]   [multiple]   [ ]                      
  * e-mail:      [optional]   [multiple]   [look-up key]            
  * nic-hdl:     [mandatory]  [single]     [primary/look-up key]    
  * remarks:     [optional]   [multiple]   [ ]                      
  * notify:      [optional]   [multiple]   [inverse key]            
  * mnt-by:      [optional]   [multiple]   [inverse key]            
  * changed:     [mandatory]  [multiple]   [ ]                      
  * source:      [mandatory]  [single]     [ ]                      
  *
  * @return String the template.
  */
  public String getTemplate(boolean CStyle) {
    String result = new String();

    String pad1 = "               "; /* XXX It is a crappy! assumption of the longest attribute name. */
    String pad2 = "           ";
    String pad3 = "           ";

    String sofl = new String();
    String eofl = new String();
    if(CStyle) {
      sofl = "\"";
      eofl = "\\n\"\n";
    }
    else {
      sofl = "";
      eofl = "\n";
    }

    AttributeDef ad;
    String name, choice, number, keytype;
    Enumeration e = attributes.elements();
    while (e.hasMoreElements()) {
      ad = (AttributeDef)e.nextElement();
      boolean pseudo = ad.getPseudo();
      if (pseudo) {
          continue;
      }
      name = ad.getName();
      choice = ad.getChoice();
      number = ad.getNumber();
      keytype = ad.getKeytype2();
      result += (sofl + name + ":"                                 +
                 pad1.substring(0, pad1.length()-name.length())    +
                 "[" + choice + "]"                                +
                 pad2.substring(0, pad2.length()-choice.length())  +
                 "[" + number + "]"                                +
                 pad3.substring(0, pad3.length()-number.length())  +
                 "[" + keytype + "]"                               +
                 eofl
                );
    }

    return result;
  } // getTemplate()

  public String getTemplateV(boolean CStyle, RPSLSyntax syntaxes) {
    String result=new String();
    String templ = getTemplate(CStyle);

    String sofl = new String();
    String eofl = new String();
    if(CStyle) {
      sofl = "\"";
      eofl = "\\n\"\n";
    }
    else {
      sofl = "";
      eofl = "\n";
    }

    result += (sofl + "The " + name + " class:" + eofl          +
               toCString(description)                           +
               sofl + eofl                                      +
               templ                                            +
               sofl + eofl                                      +
               sofl + "The content of the attributes of the "   +
               name + " class are defined below:" + eofl        +
               sofl + eofl
              );

// XXX: add formatting information
//    String name, description, format;
    Enumeration e = attributes.elements();
    while (e.hasMoreElements()) {
      AttributeDef ad = (AttributeDef)e.nextElement();
      boolean pseudo = ad.getPseudo();
      if (pseudo) {
          continue;
      }
      String name = ad.getName();
      AttributeSyntax syntax = syntaxes.getSyntaxByName(ad.getSyntax());
      if (syntax == null) {
          System.err.println("Unknown syntax '" + ad.getSyntax() + "'");
          System.exit(1);
      }
      String description;
      String syntaxDescription;
      if (CStyle) {
        description = toCString(ad.getDescription());
        syntaxDescription = toCString(syntax.getDescription());
// XXX: formatting...
//        format = toCString(ad.getFormat());
      }
      else {
        description = ad.getDescription();
        syntaxDescription = syntax.getDescription();
// XXX: formatting...
//        format = ad.getFormat();
      }

      result += (sofl + name + eofl +
                 description        +
                 syntaxDescription  +
// XXX: formatting...
//                 format             +
                 sofl + eofl     
                );
    }

/*    result += (sofl + eofl                                               +
               sofl + "Further information may be found at:" + eofl      +
               sofl + eofl                                               +
               sofl + "http://www.ripe.net/docs/ripe-157.html" + eofl    +
               sofl + "ftp://ftp.ripe.net/ripe/docs/ripe-157.ps" + eofl  +
               sofl + "ftp://ftp.ripe.net/ripe/docs/ripe-157.txt" + eofl +
               sofl + eofl
              );*/

    return result;
  } // getTemplateV()

  public String getDiagram(int maxWidth, Hashtable foreigns) {
    ClassDiagram om = new ClassDiagram(this, maxWidth, foreigns);

    return om.toString();
  } // getDiagram()

  /*
  public boolean equals(ClassDef ro) {
    return code.equals(ro.getCode());
  } // equals()
  */
    
  public String toString() {
    return new String("ripe class={" +
                         "\n\tname="         + name          +
                         "\n\tcode="         + code          +
                         "\n\tstatus="       + status        +
                         "\n\tdescription="  + description   +
                         "\n\tattributes="   + attributes    +
                         "\n\tdbaseCode="    + dbaseCode     +
                         "\n\twidth="        + width         +
                         "\n\tforeignAttrs=" + foreignAttrs  +
                         "\n}");
  } // toString()

  /**
   * RIPE class diagram.
   *
   * @author ottrey@ripe.net
   * @version $Version$
   *
   */
  public class ClassDiagram {
    private Vector    diagram;

    // -----------------oOo-----------------
    //              Constructors
    // -----------------oOo-----------------
    /**
     * Creates a RIPE class diagram.
     *               
     * @author ottrey@ripe.net
     * @version $Version$
     *               
     * @param od Definition to make diagram from.
     */
    public ClassDiagram(ClassDef od, int maxWidth, Hashtable foreigns) {
      diagram = new Vector();
      String line = new String();

      String border = new String();
      border = "+-"; 
      for (int i=0; i < getWidth(); i++) {
        border += ("-");
      }
      border += "-+   "; 
      for (int i=od.getWidth(); i < maxWidth; i++) {
        border += (" ");
      }
      Enumeration e1 = foreigns.keys();
      while (e1.hasMoreElements()) {
        String foreign = (String)e1.nextElement();
        if (e1.hasMoreElements()) {
          border += ("|--");
        }
      }
      border += ("|\n");
      diagram.addElement(border);

      line = ("| " + od.getCode() + ": " + od.getName());
      for (int i=od.getName().length()+4; i < getWidth(); i++) {
        line += (" ");
      }
      line += (" |   ");
      for (int i=od.getWidth(); i < maxWidth; i++) {
        line += (" ");
      }
      Enumeration e3 = foreigns.keys();
      while (e3.hasMoreElements()) {
        String foreign = (String)e3.nextElement();
        line += (foreign + " ");
      }
      line += ("\n");
      diagram.addElement(line);

      diagram.addElement(border);

      AttributeDef ad;
      Enumeration e = od.getAttributes().elements();
      while (e.hasMoreElements()) {
        ad = (AttributeDef)e.nextElement();
        String name = ad.getName();
        String keytype = ad.getKeytype3();
        line = ("| " + ad.getCode() + ": " + name + " ");
        for (int i=name.length(); i < width; i++) {
          line += (" ");
        }
        line += (keytype + " |");

        // The horizontal line.
        // Is a foreign key.
        boolean f = (ad.getForeign().length() > 1);
        // Is a referenced key.
        boolean p = (foreigns.contains(ad.getCode()));
        if (f) {
          line += ("->-");
        }
        else if (p) {
          line += ("-|-");
        } else {
          line += ("   ");
        }
        for (int i=od.getWidth(); i < maxWidth; i++) {
          if (f || p) {
            line += ("-");
          }
          else {
            line += (" ");
          }
        }

        // Add the xrefs.
        Enumeration e2 = foreigns.keys();
        while (e2.hasMoreElements()) {
          String foreign = (String)e2.nextElement();
          String code = ad.getCode();
          if (foreign.equals(code)) {
            line += ("X");
          }
          else if (foreigns.get(foreign).equals(code)) {
            line += ("o");
          }
          else {
            line += ("|");
          }
          if (e2.hasMoreElements()) {
            line += ("--");
          }
        }
        line += ("\n");
        diagram.addElement(line);
      }

      diagram.addElement(border);
    } // ClassDiagram()

    public String toString() {
      String result = new String();

      Enumeration e = diagram.elements();
      while (e.hasMoreElements()) {
        String line = (String)e.nextElement();
        result += line;
      }
      
      return result;
    } // toString()

  } // ClassDiagram

} // ClassDef
