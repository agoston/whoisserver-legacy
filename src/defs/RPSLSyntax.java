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

import javax.xml.parsers.*;
import org.w3c.dom.*;
import org.xml.sax.*;

import java.util.*;
import java.io.*;

public class RPSLSyntax {
    private Vector attributeSyntaxes;

    // constructor - call with an XML rpsl_syntax node
    public RPSLSyntax(Node node)
    { 
        attributeSyntaxes = new Vector();

        // load values from the XML node
        Node kid = node.getFirstChild();
        while (kid != null) 
        {
            String nodeName = kid.getNodeName();
            String nodeValue = kid.getNodeValue();
            if (nodeName.equals("attribute_syntax")) 
            {
                attributeSyntaxes.add(new AttributeSyntax(kid));
            } 
            kid = kid.getNextSibling();
        }
    }

    public int getSyntaxOfs(String name)
    {
        for (int i=0; i<attributeSyntaxes.size(); i++) {
            AttributeSyntax as=(AttributeSyntax)attributeSyntaxes.elementAt(i);
            if (as.getName().equals(name)) {
                return i;
            }
        }
        return -1;
    }

    public AttributeSyntax getSyntaxByName(String name)
    {
        for (int i=0; i<attributeSyntaxes.size(); i++) {
            AttributeSyntax as=(AttributeSyntax)attributeSyntaxes.elementAt(i);
            if (as.getName().equals(name)) {
                return as;
            }
        }
        return null;
    }

    private String escapeStringForC(String s)
    {
        StringBuffer sb = new StringBuffer();

        byte[] bytes = s.getBytes();

        for (int i=0; i<bytes.length; i++) {
            byte c = bytes[i];
            if (c == '\\') {
                sb.append("\\\\");
            } else if (c == '"') {
                sb.append("\\\"");
            } else if ((c <= 0x1F) || (c >= 0x7F)) {
                if (c < 0x10) {
                    sb.append("\\x0");
                } else {
                    sb.append("\\x");
                }
                Byte b = new Byte(c);
                sb.append(Integer.toHexString(b.intValue()));
            } else {
                sb.append(s.substring(i, i+1));
            }
        }
        return sb.toString();
    }

    private void writeAttributeInfo(PrintWriter out, AttributeSyntax a)
    {
        out.println("        /* name */");
        out.println("        \"" + a.getName() + "\",");
        out.println("        /* core_regex_pattern */");
        String coreRegex = a.getCoreRegex();
        if (coreRegex == null) {
            out.println("        NULL,");
        } else {
            out.println("        \"" + escapeStringForC(coreRegex.trim()) +
                        "\",");
        }
        out.println("        /* core_regex, set by syntax_init() */");
        out.println("        NULL,");
        out.println("        /* core_reserved_regex_pattern */");
        String coreReservedRegex = a.getCoreReservedRegex();
        if (coreReservedRegex == null) {
            out.println("        NULL,");
        } else {
            out.println("        \"" + 
                        escapeStringForC(coreReservedRegex.trim()) + "\",");
        }
        out.println("        /* core_reserved_regex, set by syntax_init() */");
        out.println("        NULL,");
        String coreParserName = a.getCoreParserName();
        out.println("        /* core_parser_reset */");
        if (coreParserName == null) {
            out.println("        NULL,");
        } else { 
            out.println("        " + coreParserName + "_reset,");
        }
        out.println("        /* core_parser */");
        if (coreParserName == null) {
            out.println("        NULL,");
        } else { 
            out.println("        " + coreParserName + "parse,");
        }
        out.println("        /* front_end_regex_pattern */");
        String regex = a.getFrontEndRegex();
        if (regex == null) {
            regex = coreRegex;
        }
        if (regex == null) {
            out.println("        NULL,");
        } else {
            out.println("        \""+escapeStringForC(regex.trim())+"\",");
        }
        out.println("        /* front_end_regex, set by syntax_init() */");
        out.println("        NULL,");
        out.println("        /* front_end_reserved_regex_pattern */");
        regex = a.getFrontEndReservedRegex();
        if (regex == null) {
            regex = coreReservedRegex;
        }
        if (regex == null) {
            out.println("        NULL,");
        } else {
            out.println("        \""+escapeStringForC(regex.trim())+"\",");
        }
        out.println("        /* front_end_reserved_regex, set by syntax_init() */");
        out.println("        NULL,");
        String parser_name = a.getFrontEndParserName();
        if (parser_name == null) {
            parser_name = coreParserName;
        }
        out.println("        /* front_end_parser_reset */");
        if (parser_name == null) {
            out.println("        NULL,");
        } else {
            out.println("        " + parser_name + "_reset,");
        }
        out.println("        /* front_end_parser */");
        if (parser_name == null) {
            out.println("        NULL ");
        } else {
            out.println("        " + parser_name + "parse");
        }
    }

    public void writeSyntaxTab(String fileName) throws IOException
    {
        PrintWriter out = new PrintWriter(new FileWriter(fileName));
        out.println("/* generated by 'RPSLSyntax.java' - DO NOT HAND-EDIT */");
        out.println("#ifndef SYNTAX_TAB_H");
        out.println("#define SYNTAX_TAB_H");
        out.println();

        Enumeration e = attributeSyntaxes.elements();
        while (e.hasMoreElements()) {
            AttributeSyntax a = (AttributeSyntax)e.nextElement();
            String parserName = a.getCoreParserName();
            if (parserName != null) {
                out.println("extern void " + parserName + "_reset();");
                out.println("extern int " + parserName + "parse();");
            }
            parserName = a.getFrontEndParserName();
            if (parserName != null) {
                out.println("extern void " + parserName + "_reset();");
                out.println("extern int " + parserName + "parse();");
            }
        }

        out.println();
        out.println("syntax_t syntax_tab[] = {");

        if (!attributeSyntaxes.isEmpty()) {
            out.println("    {");
            e = attributeSyntaxes.elements();
            writeAttributeInfo(out, (AttributeSyntax)e.nextElement());
            while (e.hasMoreElements()) {
                out.println("    },");
                out.println("    {");
                writeAttributeInfo(out, (AttributeSyntax)e.nextElement());
            }
            out.println("    }");
        }

        out.println("};");
        out.println();
        out.println("#endif /* SYNTAX_TAB_H */");
        out.close();
    }

    public void writeSyntaxMakefile(String fileName) throws IOException
    {
        PrintWriter out = new PrintWriter(new FileWriter(fileName));
        out.println("# generated by 'RPSLSyntax.java' - DO NOT HAND-EDIT");
        out.println();

        out.println("all: $(RIPLIBDIR)/librpsl.a $(RIPINCDIR)/syntax_api.h");
        out.println();

        out.println("$(RIPINCDIR)/syntax_api.h: syntax_api.h");
        out.println("\trm -f $(RIPINCDIR)/syntax_api.h");
        out.println("\tln -s syntax_api.h $(RIPINCDIR)/syntax_api.h");
        out.println();

        int numOfParser = 0;
        Enumeration e = attributeSyntaxes.elements();
        while (e.hasMoreElements()) {
            AttributeSyntax a = (AttributeSyntax)e.nextElement();
            if (a.getCoreParserName() != null) {
                numOfParser++;
            }
            if (a.getFrontEndParserName() != null) {
                numOfParser++;
            }
        }

        if (numOfParser == 0) {
            out.println("$(RIPLIBDIR)/librpsl.a: " +
                        "syntax_api.o class.o attribute.o syntax.o");
        } else {
            out.println("$(RIPLIBDIR)/librpsl.a: " +
                        "syntax_api.o class.o attribute.o syntax.o\\");
            e = attributeSyntaxes.elements();
            int parsersLeft = numOfParser;
            while (parsersLeft > 0) {
                AttributeSyntax a = (AttributeSyntax)e.nextElement();
                String parserName = a.getCoreParserName();
                if (parserName != null) {
                    out.print("        " + parserName + ".lex.o " +
                                           parserName + ".tab.o");
                    if (parsersLeft > 1) {
                        out.println(" \\");
                    } else {
                        out.println();
                    }
                    parsersLeft--;
                }
                parserName = a.getFrontEndParserName();
                if (parserName != null) {
                    out.print("        " + parserName + ".lex.o " +
                                           parserName + ".tab.o");
                    if (parsersLeft > 1) {
                        out.println(" \\");
                    } else {
                        out.println();
                    }
                    parsersLeft--;
                }
            }
        }
        out.println("\tar -r $(RIPLIBDIR)/librpsl.a $?");
        out.println("\tar -r $(RIPLIBDIR)/librip.a $?");
        out.println();

        e = attributeSyntaxes.elements();
        while (e.hasMoreElements()) {
            AttributeSyntax a = (AttributeSyntax)e.nextElement();
            String parserName = a.getCoreParserName();
            if (parserName != null) {
                out.println(parserName + ".lex.o: " + parserName + ".lex.c " +
                                                      parserName + ".tab.h");
                out.println("\t$(CC) $(CFLAGS) -c " + parserName + ".lex.c");
                out.println();

                out.println(parserName + ".tab.o: " + parserName + ".tab.c " + 
                                                      parserName + ".tab.h");
                out.println("\t$(CC) $(CFLAGS) -c " + parserName + ".tab.c");
                out.println();

                out.println(parserName + ".lex.c: " + parserName + ".l");
                out.println("\tflex -i -P" + parserName + 
                            " -o" + parserName + ".lex.c " + 
                            parserName + ".l");
                out.println();

                out.println(parserName + ".tab.c " + parserName + ".tab.h: " +
                            parserName + ".y");
                out.println("\tbison -d -p" + parserName + " " + 
                            parserName + ".y");
                out.println();
            }
            parserName = a.getFrontEndParserName();
            if (parserName != null) {
                out.println(parserName + ".lex.o: " + parserName + ".lex.c " +
                                                      parserName + ".tab.h");
                out.println("\t$(CC) $(CFLAGS) -c " + parserName + ".lex.c");
                out.println();

                out.println(parserName + ".tab.o: " + parserName + ".tab.c " + 
                                                      parserName + ".tab.h");
                out.println("\t$(CC) $(CFLAGS) -c " + parserName + ".tab.c");
                out.println();

                out.println(parserName + ".lex.c: " + parserName + ".l");
                out.println("\tflex -i -P" + parserName + 
                            " -o" + parserName + ".lex.c " + 
                            parserName + ".l");
                out.println();

                out.println(parserName + ".tab.c " + parserName + ".tab.h: " +
                            parserName + ".y");
                out.println("\tbison -d -p" + parserName + " " + 
                            parserName + ".y");
                out.println();
            }
        }
        out.close();
    }

    // when run as an application, produce the "syntax_tab.h" header, as
    // well as the Makefile.syntax to actually compile everything
    public static void main(String[] args) throws ParserConfigurationException,
                                                  SAXException,
                                                  IOException
    {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = dbf.newDocumentBuilder();
        Document doc = db.parse(new File("syntax.xml"));
        RPSLSyntax syntaxes = null;
        Node syntax_search = doc.getFirstChild();
        while (syntax_search != null) {
            if (syntax_search.getNodeName().equals("rpsl_syntax")) {
                syntaxes = new RPSLSyntax(syntax_search);
            }
            syntax_search = syntax_search.getNextSibling();
        }
        syntaxes.writeSyntaxTab("syntax_tab.h");
        syntaxes.writeSyntaxMakefile("Makefile.syntax");
    }
}

