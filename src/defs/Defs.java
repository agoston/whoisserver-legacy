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

import java.io.*;
import java.util.*;
import org.xml.sax.*;
import org.w3c.dom.*;
import javax.xml.parsers.*;

/**
 * RIPE classes generated from Data Object Models.
 *
 * @author ottrey@ripe.net
 * @version $Version$
 *
 */
public class Defs {

  // These enumurations are hard-coded in the software, so must be defined.
  // If they are not defined in the XML, they will be #defined to a bogus value.
  private static final String[] requiredClassEnum = { 
      "C_AS", "C_AN", "C_DN", "C_IR", "C_I6", "C_IN", "C_IT",
      "C_MT", "C_PN", "C_RO", "C_R6", "C_RT", "C_RS", "C_FS" 
  };
  private static final String[] requiredAttributeEnum = { 
      "A_DN", "A_FI", "A_IN", "A_I6", "A_LA", "A_MO", "A_NA", 
      "A_NH", "A_PN", "A_RT"
  };


  // hash of attribute xmlnames to attribute objects
  private Hashtable xmlNameToAttribute;
  // hash of attribute names to objects
  private Hashtable nameToAttribute;
  // hash of attribute codes to objects
  private Hashtable codeToAttribute;
  private Vector ripeAttributes;
  private Vector ripeAttributeCodes; 
  private Vector ripeClasses;
  private Vector ripeAttributeAliases;
  private Vector ripeAttributeAliasesMap;
  private Vector ripeClassAliases;
  private Vector ripeClassAliasesMap;
  private Vector ripeQueries;
  private RPSLSyntax syntaxes;


  // -----------------oOo-----------------
  //              Constructors
  // -----------------oOo-----------------
  public Defs(boolean normalize) {
    Document syntaxDOM = getDOM("syntax.xml", normalize);
    Node syntax_search = syntaxDOM.getFirstChild();
    while (syntax_search != null) {
        if (syntax_search.getNodeName().equals("rpsl_syntax")) {
            syntaxes = new RPSLSyntax(syntax_search);
        }
        syntax_search = syntax_search.getNextSibling();
    }

    // Create a normalized DOM from the xml file for the attributes.
    Document attrDOM = getDOM("attributes.xml", normalize);

    // Initialize.
    xmlNameToAttribute = new Hashtable();
    nameToAttribute = new Hashtable();
    codeToAttribute = new Hashtable();
    ripeAttributes = new Vector();
    ripeAttributeCodes = new Vector();
    ripeAttributeAliases = new Vector();
    ripeAttributeAliasesMap = new Vector();
    ripeClassAliases = new Vector();
    ripeClassAliasesMap = new Vector();
    ripeQueries = new Vector();

    // Recurse through node tree
    NodeList attrNodes = attrDOM.getElementsByTagName("ripe_attribute");
    for (int i=0; i < attrNodes.getLength(); i++) {
      // (Checking if the attribute is valid)
      if (validate("attribute", attrNodes.item(i))) {
        AttributeDef ad = new AttributeDef(attrNodes.item(i));

        ripeAttributes.addElement(ad);
        
        // and each attribute,
        //ripeAttributes.put(ad.getCode(), ad);
        xmlNameToAttribute.put(ad.getXmlName(), ad);

        //
        // only add the information if this is the first time for the attribute
        //
        if (!nameToAttribute.containsKey(ad.getName()))
        {
            // add the name of the attribute
            nameToAttribute.put(ad.getName(), ad);

            // and it's code.
            ripeAttributeCodes.addElement(ad.getCode());
            codeToAttribute.put(ad.getCode(), ad);

            // and it's aliases.
            // also map the alias to the attribute index.
 
            // set the index to map to.
            Integer mapIndex = new Integer(ripeAttributeCodes.size()-1);

            //  first the code.
            ripeAttributeAliases.addElement(ad.getCode());
            ripeAttributeAliasesMap.addElement(mapIndex);

            //  then the name.
            ripeAttributeAliases.addElement(ad.getName());
            ripeAttributeAliasesMap.addElement(mapIndex);

            if (ad.getAltName().length() > 1) {
              //  then the altName.
              ripeAttributeAliases.addElement(ad.getAltName());
              ripeAttributeAliasesMap.addElement(mapIndex);
            }
        } // endif (!nameToAttribute.containsKey(ad.getName()) 
      }
    }

    // Create a normalized DOM from the xml file for the classes.
    Document objDOM = getDOM("classes.xml", normalize);

    // Create a vector to store the classes.
    ripeClasses = new Vector();

    // Recurse through node tree
    NodeList objNodes = objDOM.getElementsByTagName("ripe_class");
    for (int i=0; i < objNodes.getLength(); i++) {
      // Check if the class is valid
      if (validate("class", objNodes.item(i))) {
        ClassDef od = new ClassDef(objNodes.item(i), xmlNameToAttribute);
        
        // Add the class.
//        ripeClasses.addElement(od);
        if (ripeClasses.size() <= od.getDbaseCode()) {
            ripeClasses.setSize(od.getDbaseCode()+1);
        } 
        ripeClasses.setElementAt(od, od.getDbaseCode());

        // set the index to map to.
//        Integer mapIndex = new Integer(ripeClasses.size()-1);
        Integer mapIndex = new Integer(od.getDbaseCode());

        //  first the code.
        ripeClassAliases.addElement(od.getCode());
//        ripeClassAliases.setElementAt(od, od.getDbaseCode());
        ripeClassAliasesMap.addElement(mapIndex);

        //  then the name.
        ripeClassAliases.addElement(od.getName());
//        ripeClassAliases.setElementAt(od.getName(), od.getDbaseCode());
        ripeClassAliasesMap.addElement(mapIndex);

      }
    }
    
    // replace class/attribute variables in queries
    


  } // Defs()
    
    public String getValueByEnum(String name) {
	Enumeration e = ripeClasses.elements();
	for( int i = 0; e.hasMoreElements();  i++) {
	    ClassDef d = (ClassDef)e.nextElement();

            // Handle missing numbers
            if (d == null) {
                return "-1";
            }

	    String a = d.getEnum();
	    
	    //System.out.println( d );
	    
	    if( name.equals(a) ) {
		return (new Integer(i)).toString();
	    }
	}
	System.err.println("ERROR: cannot resolve variable name " + name );
	System.exit(-1);
	
	return ""; // bloody idiot, the compiler
    }
	    
	    

  /** 
   * Creates a Data Object Model from the RIPE classes defined
   * in the XML document.
   *               
   * @author ottrey@ripe.net
   * @version $Version$
   *               
   * @param xmlDocName The URI of the XML document.
   * @param ripeClass  The class to be created from.
   * @param normalize  Return a normalized DOM.
   *
   */
  private Document getDOM(String xmlDocName, boolean normalize) {

    DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
    DocumentBuilder db = null;
    try {
        db = dbf.newDocumentBuilder();
    } catch (ParserConfigurationException pce) {
        System.err.println(pce);
        System.exit(1);
    }


    Document dom = null;
    try {
        dom = db.parse(new File(xmlDocName));
    } catch (SAXException se) {
        System.err.println(se.getMessage());
        System.exit(1);
    } catch (IOException ioe) {
        System.err.println(ioe);
        System.exit(1);
    }

    // Normalize the document.
    if (normalize) {
      dom.getDocumentElement().normalize();
    }

    return dom;

  } // getDOM()

  private boolean validate(String type, Node obj) {
    boolean result=false;
    String status = obj.getAttributes().getNamedItem("status").getNodeValue(); 
    String name   = obj.getAttributes().getNamedItem("name").getNodeValue();

    if (status.equals("valid")) {
      result=true;
    }
    else {
      System.err.println("Warning: " + type + " " + name + " is " + status);
    }

    return result;
  } // validClass()


  // -----------------oOo-----------------
  //              Print Methods
  // -----------------oOo-----------------
  private void printDF_attribute_aliases() {
    System.out.println("char * const Attribute_aliases[] = {");
    Enumeration e = ripeAttributeAliases.elements();
    while (e.hasMoreElements()) {
      String a = (String)e.nextElement();
      System.out.println("  \"" + a + "\",");
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Attribute_aliases */");
  } // printDF_attribute_aliases()

  private void printDF_attribute_aliases_map() {
    System.out.println("int const Attribute_aliases_map[] = {");
    Enumeration e = ripeAttributeAliasesMap.elements();
    while (e.hasMoreElements()) {
      Integer am = (Integer)e.nextElement();
      System.out.println("  " + am + ",");
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Attribute_aliases_map */");
  } // printDF_attribute_aliases_map()

  private void printDF_attribute_codes() {
    System.out.println("char * const Attribute_codes[] = {");
    Enumeration e = ripeAttributeCodes.elements();
    while (e.hasMoreElements()) {
      String ac = (String)e.nextElement();
      //AttributeDef ad = (AttributeDef)ripeAttributes.get(ac);
      AttributeDef ad = (AttributeDef)codeToAttribute.get(ac);

      // If the attribute has status="valid".
      if (ad.getStatus().equals("valid")) {
        // Output the attribute code.
        System.out.println("  \"" + ad.getCode() + "\",");
      }
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Attribute_codes */");
  } // printDF_attribute_codes()

  private void printDF_attribute_enum() {
    System.out.println("typedef enum _A_Type_t {");

    // keep track of enums we've seen to make sure we get all of the 
    // required ones
    boolean[] requiredSeen = new boolean[requiredAttributeEnum.length];

    // Enumerate through the attribute codes.
    Enumeration e = ripeAttributeCodes.elements();
    while (e.hasMoreElements()) {
      String ac = (String)e.nextElement();
      //AttributeDef ad = (AttributeDef)ripeAttributes.get(ac);
      AttributeDef ad = (AttributeDef)codeToAttribute.get(ac);

      // If the attribute has status="valid".
      if (ad.getStatus().equals("valid")) {
        // Output the attribute enum.
        System.out.println("  " + ad.getEnum() + ",");

        for (int i=0; i<requiredAttributeEnum.length; i++) {
            if (requiredAttributeEnum[i].equals(ad.getEnum())) {
                requiredSeen[i] = true;
            }
        }
      }
    }
    System.out.println("  " + "A_END" + "\n" + "} A_Type_t;\n");

    int bogusEnum = 128;
    for (int i=0; i<requiredAttributeEnum.length; i++) {
        if (!requiredSeen[i]) {
            bogusEnum--;
            System.out.println("#define " + requiredAttributeEnum[i] + " " +
                bogusEnum + "    /* XXX: attribute required by software */");
            System.out.println("#define NO_" + requiredAttributeEnum[i]);
        }
    }

  } // printDF_attribute_enum()

  private void printDF_attribute_inv_attr_mask() {
    System.out.print("#define INV_ATTR_MASK ");

    // Enumerate through the attribute codes.
    Enumeration e = ripeAttributeCodes.elements();
    while (e.hasMoreElements()) {
      String ac = (String)e.nextElement();
      //AttributeDef ad = (AttributeDef)ripeAttributes.get(ac);
      AttributeDef ad = (AttributeDef)codeToAttribute.get(ac);

      // If the attribute has status="valid".
      if (ad.getStatus().equals("valid") && ad.getInverse()) {
        // Output the attribute enum.
        System.out.print(ad.getEnum() + ", ");
      }
    }

    System.out.println("MA_END");
  } // printDF_attribute_inv_attr_mask()

  private void printDF_attribute_names() {
    System.out.println("char * const Attribute_names[] = {");
    Enumeration e = ripeAttributeCodes.elements();
    while (e.hasMoreElements()) {
      String ac = (String)e.nextElement();
      //AttributeDef ad = (AttributeDef)ripeAttributes.get(ac);
      AttributeDef ad = (AttributeDef)codeToAttribute.get(ac);

      // If the attribute has status="valid".
      if (ad.getStatus().equals("valid")) {
        // Output the attribute name.
        System.out.println("  \"" + ad.getName() + "\",");
      }
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Attribute_names */");
  } // printDF_attribute_names()

  private void printDF_class_aliases() {
    System.out.println("char * const Class_aliases[] = {");
    Enumeration e = ripeClassAliases.elements();
    while (e.hasMoreElements()) {
      String a = (String)e.nextElement();
      System.out.println("  \"" + a + "\",");
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Class_aliases */");
  } // printDF_class_aliases()

  private void printDF_class_main_tables() {
    System.out.println("char * const Type2main[] = {");
    Enumeration e = ripeClasses.elements();
    while (e.hasMoreElements()) {
      ClassDef cd = (ClassDef)e.nextElement();
      if (cd == null) {
          System.out.println("  9999,   /* XXX: no main table map */");
      } else {
          System.out.println("  \"" + cd.getMainTable() + "\",");
      }
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Class_main_tables */");
  } // printDF_class_main_tables()


  private void printDF_class_aliases_map() {
    System.out.println("int const Class_aliases_map[] = {");
    Enumeration e = ripeClassAliasesMap.elements();
    while (e.hasMoreElements()) {
      Integer am = (Integer)e.nextElement();
      System.out.println("  " + am + ",");
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Class_aliases_map */");
  } // printDF_class_aliases_map()

  private void printDF_class_codes() {
    System.out.println("char * const Class_codes[] = {");
    Enumeration e = ripeClasses.elements();
    while (e.hasMoreElements()) {
      ClassDef od = (ClassDef)e.nextElement();
      if (od == null) {
          System.out.println("  \"\",   /* XXX: no class code */");
      } else {
          System.out.println("  \"" + od.getCode() + "\",");
      }
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Class_codes */");
  } // printDF_class_codes()

  private void printDF_class_dbase_code_map() {
    System.out.println("int const Class_dbase_code_map[] = {");
    Enumeration e = ripeClasses.elements();
    while (e.hasMoreElements()) {
      ClassDef cd = (ClassDef)e.nextElement();
      if (cd == null) {
          System.out.println("  9999,   /* XXX: no class code to map */");
      } else {
          System.out.println("  " + cd.getDbaseCode() + ",");
      }
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Class_dbase_code_map */");
  } // printDF_class_dbase_code_map()

  private void printDF_class_enum() {
    System.out.println("typedef enum _C_Type_t {");
    Enumeration e = ripeClasses.elements();

    // keep track of enums we've seen to make sure we get all of the 
    // required ones
    boolean[] requiredSeen = new boolean[requiredClassEnum.length];

    int num_bogus = 0;
    System.out.println("  C_ANY = -1, ");
    while (e.hasMoreElements()) {
      ClassDef od = (ClassDef)e.nextElement();
      if (od == null) {
          System.out.println("  C_BOGUS" + num_bogus + 
                             ",  /* XXX: no class code to enumerate */");
          num_bogus++;
      } else {
          System.out.println("  " + od.getEnum() + ",");
          for (int i=0; i<requiredClassEnum.length; i++) {
              if (requiredClassEnum[i].equals(od.getEnum())) {
                  requiredSeen[i] = true;
              }
          }
      }
    }
    System.out.println("  " + "C_END" + "\n" + "} C_Type_t;\n");

    int bogusEnum = 128;
    for (int i=0; i<requiredClassEnum.length; i++) {
        if (!requiredSeen[i]) {
            bogusEnum--;
            System.out.print("#define " +requiredClassEnum[i] +" " +bogusEnum);
            System.out.println("    /* XXX: class required by software */");
            System.out.println("#define NO_" + requiredClassEnum[i]);
        }
    }
  } // printDF_class_enum()

  private void printDF_class_mask() {
    int num_bogus = 0;
    System.out.print("#define CLASS_MASK ");
    Enumeration e = ripeClasses.elements();
    while (e.hasMoreElements()) {
      ClassDef cd = (ClassDef)e.nextElement();
      if (cd == null) {
          System.out.print("C_BOGUS" + num_bogus + ", ");
          num_bogus++;
      } else {
          System.out.print(cd.getEnum() + ", ");
      }
    }
    System.out.println("MA_END");
  } // printDF_class_mask()

  private void printDF_class_names() {
    System.out.println("char * const Class_names[] = {");
    Enumeration e = ripeClasses.elements();
    while (e.hasMoreElements()) {
      ClassDef cd = (ClassDef)e.nextElement();
      if (cd == null) { 
          System.out.println("  \"\",    /* XXX: no class to name */");
      } else {
          System.out.println("  \"" + cd.getName() + "\",");
      }
    }
    System.out.println("  " + "NULL" + "\n" + "}; /* Class_names */");
  } // printDF_class_names()

  private void printQI_queries() {
    System.out.println(Query.startDoc());
    //Enumeration e1 = ripeAttributes.elements();
    Enumeration e1 = nameToAttribute.elements();
    while (e1.hasMoreElements()) {
      AttributeDef ad = (AttributeDef)e1.nextElement();
      Enumeration e2 = ad.getQueries().elements();
      while (e2.hasMoreElements()) {
        Query q = (Query)e2.nextElement();
        System.out.println(q.getStruct("  ", this));
      }
    }
    System.out.println(Query.endDoc());
  } // printQI_queries()

  private void printUD_queries() {

    Enumeration e;

    System.out.println("UD_query Insert[] = {");
    e = ripeAttributeCodes.elements();
    while (e.hasMoreElements()) {
      String ac = (String)e.nextElement();
      //AttributeDef ad = (AttributeDef)ripeAttributes.get(ac);
      AttributeDef ad = (AttributeDef)codeToAttribute.get(ac);
      System.out.println("  {" + ad.getInsertQ_type() + ", " + "\"" +  ad.getInsert() + "\"},");
    }
    System.out.println("  " + "{0, NULL}" + "\n" + "}; /* Insert */\n");


    System.out.println("UD_query Update[] = {");
    e = ripeAttributeCodes.elements();
    while (e.hasMoreElements()) {
      String ac = (String)e.nextElement();
      //AttributeDef ad = (AttributeDef)ripeAttributes.get(ac);
      AttributeDef ad = (AttributeDef)codeToAttribute.get(ac);
      System.out.println("  {" + ad.getUpdateQ_type() + ", " + "\"" +  ad.getUpdate() + "\"},");
    }
    System.out.println("  " + "{0, NULL}" + "\n" + "}; /* Update */\n");

    System.out.println("UD_query Dummy[] = {");
    e = ripeAttributeCodes.elements();
    while (e.hasMoreElements()) {
      String ac = (String)e.nextElement();
      //AttributeDef ad = (AttributeDef)ripeAttributes.get(ac);
      AttributeDef ad = (AttributeDef)codeToAttribute.get(ac);
      System.out.println("  {" + ad.getDummyQ_type() + ", " + "\"" +  ad.getDummy() + "\"},");
    }
    System.out.println("  " + "{0, NULL}" + "\n" + "}; /* Dummy */\n");

    System.out.println("UD_query Select[] = {");
    e = ripeAttributeCodes.elements();
    while (e.hasMoreElements()) {
      String ac = (String)e.nextElement();
      //AttributeDef ad = (AttributeDef)ripeAttributes.get(ac);
      AttributeDef ad = (AttributeDef)codeToAttribute.get(ac);
      System.out.println("  {" + ad.getSelectQ_type() + ", " + "\"" +  ad.getSelect() + "\"},");
    }
    System.out.println("  " + "{0, NULL}" + "\n" + "}; /* Select */\n");

  } // printUD_queries()

  // XXX: unused, unloved, needs to check for null
  /* private void printTemplates() {
    Enumeration e = ripeClasses.elements();
    while (e.hasMoreElements()) {
      ClassDef cd = (ClassDef)e.nextElement();
      System.out.println(cd.getName() + "\n");
      System.out.println(cd.getTemplate(false) + "\n");
    }
  } // printTemplates() */

  private void printDF_class_templates() {
    Enumeration e = ripeClasses.elements();
    System.out.println("const char *Templates[] = {");
    while (e.hasMoreElements()) {
      ClassDef cd = (ClassDef)e.nextElement();
      if (cd == null) {
          System.out.println("\"\", /* XXX: missing class, no template */");
      } else {
          System.out.println(cd.getTemplate(true) + ",");
      }
    }
    System.out.println("NULL");
    System.out.println("}; /* Templates */");
  } // printDF_class_templates()

  private void printDF_class_templates_v() {
    Enumeration e = ripeClasses.elements();
    System.out.println("const char *Templates_v[] = {");
    while (e.hasMoreElements()) {
      ClassDef od = (ClassDef)e.nextElement();
      if (od == null) {
          System.out.println("\"\", /* XXX: missing class, no template */");
      } else {
          System.out.println(od.getTemplateV(true, syntaxes) + ",");
      }
    }
    System.out.println("NULL");
    System.out.println("}; /* Templates_v */");
  } // printDF_class_templates_v()

  // XXX: unused, unloved, needs to check for null
  /* private void printTemplatesV() {
    Enumeration e = ripeClasses.elements();
    while (e.hasMoreElements()) {
      ClassDef od = (ClassDef)e.nextElement();
      System.out.println(od.getName() + "\n");
      System.out.println(od.getTemplateV(false, syntaxes) + "\n");
    }
  } // printTemplatesV() */

  private void printDiagrams() {
    int maxWidth=0;  // Widest diagram
    Hashtable foreigns = new Hashtable();

    Enumeration e1 = ripeClasses.elements();
    while (e1.hasMoreElements()) {
      ClassDef od = (ClassDef)e1.nextElement();
      if (od != null) {
          if (maxWidth < od.getWidth()) {
            maxWidth = od.getWidth();
          }
    
          Hashtable foreignAttrs = od.getForeignAttrs();
          if (foreignAttrs != null) {
            Enumeration e2 = foreignAttrs.keys();
            while (e2.hasMoreElements()) {
              String key = (String)e2.nextElement();
              if (!foreigns.containsKey(key)) {
                foreigns.put(key, foreignAttrs.get(key));
              }
            }
          }
      }
    }

    System.out.print("Classes:");
    for (int i=0; i < maxWidth; i++) {
      System.out.print(" ");
    }
    System.out.println("Foreign keys:");

    Enumeration e3 = ripeClasses.elements();
    while (e3.hasMoreElements()) {
      ClassDef od = (ClassDef)e3.nextElement();
      if (od != null) {
          System.out.print(od.getDiagram(maxWidth, foreigns));
      }
    }
  } // printDiagrams()

  private void printDF_radix_load() {
    System.out.print("DF_Load_t DF_radix_load[] = \n{\n");

    // Enumerate through the attribute codes.
    Enumeration e = ripeAttributeCodes.elements();
    while (e.hasMoreElements()) {
      String ac = (String)e.nextElement();
      //AttributeDef ad = (AttributeDef)ripeAttributes.get(ac);
      AttributeDef ad = (AttributeDef)codeToAttribute.get(ac);

      // If the attribute has status="valid".
      if (ad.getFamily() != null) {
	  String ip4 = ad.getV4Load() != null 
	      ? "\"" + ad.getV4Load() + "\"" : "NULL";
	  String ip6 = ad.getV6Load() != null 
	      ? "\"" + ad.getV6Load() + "\"" : "NULL";

	  System.out.print("  { " +  ad.getEnum() 
			   + ", "   + ad.getFamily() 
			   + ",\n\t"  + ip4.replace('\n',' ')
			   + ",\n\t"  + ip6.replace('\n',' ')
			   + "\n  },\n");
      }
    } // while more


    System.out.println("  { -1, -1, NULL, NULL }\n};");
  } // printDF_radix_load() 

    private void writeAttributeInfo(PrintStream out, AttributeDef ad, int id)
    {
        out.println("        /* name */");
        out.println("        \"" + ad.getName() + "\",");
        out.println("        /* id */");
        out.println("        " + id + ",");
        out.println("        /* altname */");
        String altname = ad.getAltName();
        if ((altname == null) || (altname.length() == 0)) { 
            out.println("        NULL,");
        } else {
            out.println("        \"" + altname + "\",");
        }
        out.println("        /* xmlname */");
        out.println("        \"" + ad.getXmlName() + "\",");
        out.println("        /* code */");
        out.println("        \"" + ad.getCode() + "\",");
        out.println("        /* syntax_offset */");
        out.println("        " + syntaxes.getSyntaxOfs(ad.getSyntax()) + 
                    ",   /* " + ad.getSyntax() + " */");
        out.println("        /* is_lookup */");
        if (ad.getLookup()) {
            out.println("        1,");
        } else {
            out.println("        0,");
        }
        out.println("        /* is_inverse */");
        if (ad.getInverse()) {
            out.println("        1,");
        } else {
            out.println("        0,");
        }
        out.println("        /* is_primary */");
        if (ad.getPrimary()) {
            out.println("        1,");
        } else {
            out.println("        0,");
        }
        out.println("        /* is_list */");
        if (ad.getList()) {
            out.println("        1,");
        } else {
            out.println("        0,");
        }
        out.println("        /* is_ripe_list */");
        if (ad.getRipeList()) {
            out.println("        1,");
        } else {
            out.println("        0,");
        }
        out.println("        /* foreign_code */");
        String foreign = ad.getForeign();
        if ((foreign == null) || (foreign.length() == 0)) {
            out.println("        NULL,");
        } else {
            out.println("        \"" + foreign + "\"");
        }
    }

    // create attribute_tab.h
    private void printAttributeTab()
    {
        PrintStream out = System.out;

        out.println("/* generated by 'Defs.java' - DO NOT HAND-EDIT */");
        out.println("#ifndef ATTRIBUTE_TAB_H");
        out.println("#define ATTRIBUTE_TAB_H");
        out.println();
        out.println("attribute_t attribute_tab[] = {");

        if (!ripeAttributeCodes.isEmpty()) {
            Hashtable attributeIds = new Hashtable();

            out.println("    {");
            //Enumeration e = ripeAttributeCodes.elements(); 
            //String code = (String)e.nextElement();
            Enumeration e = ripeAttributes.elements(); 
            //AttributeDef ad = (AttributeDef)ripeAttributes.get(code);
            AttributeDef ad = (AttributeDef)e.nextElement();
            int id = 0;
            writeAttributeInfo(out, ad, id);
            attributeIds.put(ad.getName(), new Integer(id));
            while (e.hasMoreElements()) {
                out.println("    },");
                out.println("    {");
                //code = (String)e.nextElement();
                //ad = (AttributeDef)ripeAttributes.get(code);
                ad = (AttributeDef)e.nextElement();
                if (attributeIds.containsKey(ad.getName())) {
                    // if already have an id for this name, use that 
                    Integer prevId = (Integer)attributeIds.get(ad.getName());
                    writeAttributeInfo(out, ad, prevId.intValue());
                } else {
                    // increase id for each unique attribute name
                    id++;
                    writeAttributeInfo(out, ad, id);
                    attributeIds.put(ad.getName(), new Integer(id));
                }
            }
            out.println("    }");
        }

        out.println("};");
        out.println();
        out.println("#endif /* ATTRIBUTE_TAB_H */");
        out.close();
    }

    private int getAttrOfs(String xmlName)
    {
        for (int i=0; i<ripeAttributes.size(); i++) {
            AttributeDef ad = (AttributeDef)ripeAttributes.elementAt(i);
            if (ad.getXmlName().equals(xmlName)) {
                return i;
            }
        }
        return -1;
    }

    private void writeClassInfo(PrintStream out, ClassDef cd, int id)
    {
        /* leave an empty structure if we don't have a class here */
        if (cd == null) {
            out.println("        /* XXX: missing class */");
            out.println("        /* name */");
            out.println("        \"\",");
            out.println("        /* id */");
            out.println("        " + id + ",");
            out.println("        /* num_attr */");
            out.println("        0,");
            out.println("        /* attr_hash (set by class_init()) */");
            out.println("        NULL");
            return;
        }

        out.println("        /* name */");
        out.println("        \"" + cd.getName() + "\",");
        out.println("        /* id */");
        out.println("        " + id + ",");
        out.println("        /* num_attr */");
        Vector attributes = cd.getAttributes();
        out.println("        " + attributes.size() + ",");
        out.println("        /* attr[] */");
        out.println("        {");
        for (int i=0; i<attributes.size(); i++) {
            AttributeDef ad = (AttributeDef)attributes.elementAt(i);
            String name = ad.getName();
            String xmlName = ad.getXmlName();
            out.println("            /* attribute: " + name + 
                        " (" + xmlName + ") */");
            out.println("            {");
            int offset = getAttrOfs(xmlName);
            out.println("                /* offset */");
            out.println("                " + offset + ",");
            String choice;
            if (ad.getChoice().equals("mandatory")) {
                choice = "ATTR_MANDATORY";
            } else if (ad.getChoice().equals("optional")) {
                choice = "ATTR_OPTIONAL";
            } else {
                choice = "ATTR_GENERATED";
            }
            out.println("                /* choice */");
            out.println("                " + choice + ",");
            String number;
            if (ad.getNumber().equals("single")) {
                number = "ATTR_SINGLE";
            } else {
                number = "ATTR_MULTIPLE";
            }
            out.println("                /* number */");
            out.println("                " + number);
            out.print("            }");
            if (i != attributes.size()-1) {
                out.print(",");
            }
            out.println();
        }
        out.println("        },");
        out.println("        /* attr_hash (set by class_init()) */");
        out.println("        NULL");
    }

    // create class_tab.h
    private void printClassTab()
    {
        PrintStream out = System.out;

        out.println("/* generated by 'Defs.java' - DO NOT HAND-EDIT */");
        out.println("#ifndef CLASS_TAB_H");
        out.println("#define CLASS_TAB_H");
        out.println();
        out.println("class_t class_tab[] = {");

        if (!ripeClasses.isEmpty()) {
            out.println("    {");
            Enumeration e = ripeClasses.elements();
            ClassDef cd = (ClassDef)e.nextElement();
            int id = 0;
            writeClassInfo(out, cd, id);
            while (e.hasMoreElements()) {
                out.println("    },");
                out.println("    {");
                cd = (ClassDef)e.nextElement();
                id++;
                writeClassInfo(out, cd, id);
            }
            out.println("    }");
        }

        out.println("};");
        out.println();
        out.println("#endif /* CLASS_TAB_H */");
        out.close();
    }

    /*
     * Generate header file for the commit-rollback code.
     *
     * From the original documentation in the static version of this header file:

        This is needed for commit or rollback of the transaction (v3 software
        doesn't rely on transaction support from the underlying RDBMS; MySQL
        doesn't have it at all). The transaction in progress can be identified
        by thread_id field not being equal 0 in corresponding tables. According
        to the value of this field (insert or  update) the commit/rollback
        routine either deletes or updates the record.

        The arrays have the following format: first come tables that may be
        affected when dummy object is created to resolve references. For
        example, if one creates a inetnum object that has no corresponding
        admin-c, tech-c, mnt-by, etc., dummy records will be created in
        person_role table and mntner table. We need to clean up them.

        Secondly (starting with TAB_START) come tables that may be affected by
        the object itself. Basically it is a list of all possible attributes of
        the object of the type that are stored in the db. I guess this may be
        also derived from xml in the future.

        And NULL is a delimiter; it is also used for padding.

    */
    private void printUdComrol()
    {
        // TODO move this to wherever other constants are defined
        int tab_start = 6;
        ClassDef cd;
        Enumeration e;
        Vector tables;
        String table_name;
        int table_size;
        
        PrintStream out = System.out;

        out.println("/* generated by 'Defs.java' - DO NOT HAND-EDIT */");
        out.println("#ifndef UD_COMROL_DEF");
        out.println("#define UD_COMROL_DEF");
        out.println();
        out.println("#define TAB_START " + tab_start);
        out.println();

        if (!ripeClasses.isEmpty()) {
            e = ripeClasses.elements();
            // loop thru all classes
            do {
                cd = (ClassDef)e.nextElement();

                out.println("/* " + cd.getName() + " */");
                out.println();

                out.println("char *t_" + cd.getCode() + "[] = {");
                out.print("  ");

                tables = cd.getRollbackTables();

                table_size = 0;
                
                // Start rollback table definition
                if(tables != null)
                    table_size = tables.size();
                else 
                    out.println("#warning No rollback tables defined for class " + cd.getName());

                for (int i=0; i < tab_start; i++) {
                    if(i < table_size){
                        table_name = (String) tables.elementAt(i);
                        out.print("\"" + table_name + "\"");
                    } else {
                        out.print("NULL");
                    }
                    out.print(",");
                }
              
                out.println();
                out.print("  ");

                // Start affected table definition
                table_size = 0;
                tables = cd.getAffectedTables();

                if(tables != null)
                    table_size = tables.size();
                else
                    out.println("#warning No affected tables defined for class " + cd.getName());

                for (int i=0; i < table_size; i++) {
                    table_name = (String) tables.elementAt(i);
                    out.print("\"" + table_name + "\", ");
                }
                out.println("NULL");
                out.println("};");

                tables = cd.getInvQueryTables();

                // The next table is used for inverse querying when deleting an object
                if(tables != null){
                    out.println();
                    out.println("char *t_i" + cd.getCode() + "[] = {");
                    out.print("  ");
                    for (int i=0; i < tables.size(); i++) {
                        table_name = (String) tables.elementAt(i);
                        out.print("\"" + table_name + "\",");
                    }
                    out.println("NULL");
                    out.println("};");
                }
                
                out.println();

                
            } while (e.hasMoreElements());

            // Collect the arrays generated above into one table of references
            out.println();
            out.println("char **tables[] = {");
            e = ripeClasses.elements();

            do {
                cd = (ClassDef)e.nextElement();
                out.println("  t_" + cd.getCode() + ",");
            } while (e.hasMoreElements());

            out.println("  NULL");
            out.println("};");

        }

        out.println();
        out.println("#endif /* UD_COMROL_DEF */");
        out.close();
    }


  // -----------------oOo-----------------
  //            Unit test driver
  // -----------------oOo-----------------
  public static void main (String argv[]) {
    int n=0;
    boolean err=true;
    boolean normalize=false;

    if (argv.length > 0) {
      try {
        n = Integer.parseInt(argv[0]);
        err=false;
      }
      catch (NumberFormatException e) {
      }
      if (argv.length == 2) {
        if (argv[1].equals("normalize")) {
          normalize = true;
        }
      }
    }

    if (!err) {
      Defs rc = new Defs(normalize);

      switch (n) {
        case  1: rc.printDF_attribute_aliases();          break; 
        case  2: rc.printDF_attribute_aliases_map();      break; 
        case  3: rc.printDF_attribute_codes();            break; 
        case  4: rc.printDF_attribute_enum();             break; 
        case  5: rc.printDF_attribute_inv_attr_mask();    break; 
        case  6: rc.printDF_attribute_names();            break; 
        case  7: rc.printDF_class_aliases();              break; 
        case  8: rc.printDF_class_aliases_map();          break; 
        case  9: rc.printDF_class_codes();                break; 
        case 10: rc.printDF_class_dbase_code_map();       break; 
        case 11: rc.printDF_class_enum();                 break; 
        case 12: rc.printDF_class_mask();                 break; 
        case 13: rc.printDF_class_names();                break; 
        case 14: rc.printQI_queries();                    break; 
        case 15: rc.printUD_queries();                    break; 
        case 16: rc.printDF_class_templates();            break; 
        case 17: rc.printDF_class_templates_v();          break; 
        case 18: rc.printDiagrams();                      break; 
//        case 19: rc.printTemplates();                     break; 
//        case 20: rc.printTemplatesV();                    break; 
        case 21: rc.printDF_radix_load();                 break; 
        case 22: rc.printAttributeTab();                  break; 
        case 23: rc.printClassTab();                      break; 
        case 24: rc.printUdComrol();                      break; 
        default:
          err=true;
      }
    }

    if (err) {
      System.err.println("Usage: makedefs n (Where n = a number 1..23)");
    }

  } // main()
  // -----------------oOo-----------------

} // Defs
