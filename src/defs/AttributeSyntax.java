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

public class AttributeSyntax {

    // attribute syntax information
    private String name;             // name of the syntax, e.g. nic-handle
    private String description;      // explanatory description of syntax

    // info for core processing (whois_rip)
    private String coreRegex;          // POSIX regular expression of match
    private String coreReservedRegex;  // POSIX regular expression of reserved
                                       // words (this must NOT match)
    private String coreParserName;     // Name used by lex, yacc, etc.

    // info for front-end processing (dbupdate, etc.)
    private String frontEndRegex;         // POSIX regular expression of match
    private String frontEndReservedRegex; // POSIX regular expression of 
                                          // reserved words (that may NOT match)
    private String frontEndParserName;    // Name used by lex, yacc, etc.

    // constructor - call with an XML attribute_syntax node
    public AttributeSyntax(Node node)
    {
        // default values (these will be checked later)
        name = null;
        description = "";
        coreRegex = null;
        coreReservedRegex = null;
        coreParserName = null;
        frontEndRegex = null;
        frontEndReservedRegex = null;
        frontEndParserName = null;

        Node nameNode = node.getAttributes().getNamedItem("name");
        if (nameNode == null) 
        {
            System.err.println("Attribute syntax has no name");
            System.exit(1);
        }
        name = nameNode.getNodeValue();

        // load values from the XML node
        Node kid = node.getFirstChild();
        while (kid != null) 
        {
            String nodeName = kid.getNodeName();
            String nodeValue = getText(kid);
            if (nodeName.equals("description")) 
            {
                description += nodeValue;
            } 
            else if (nodeName.equals("core")) 
            {
                Node core = kid.getFirstChild();
                while (core != null) 
                {
                    String coreName = core.getNodeName();
                    String coreValue = getText(core);
                    if (coreName.equals("regex")) 
                    {
                        coreRegex = coreValue;
                    } 
                    else if (coreName.equals("reserved_regex")) 
                    {
                        coreReservedRegex = coreValue;
                    }
                    else if (coreName.equals("parser_name")) 
                    {
                        coreParserName = coreValue;
                    }
                    core = core.getNextSibling();
                }
            } 
            else if (nodeName.equals("front_end")) 
            {
                Node fe = kid.getFirstChild();
                while (fe != null) 
                {
                    String feName = fe.getNodeName();
                    String feValue = getText(fe);
                    if (feName.equals("regex")) 
                    {
                        frontEndRegex = feValue;
                    } 
                    else if (feName.equals("reserved_regex")) 
                    {
                        frontEndReservedRegex = feValue;
                    }
                    else if (feName.equals("parser_name")) 
                    {
                        frontEndParserName = feValue;
                    }
                    fe = fe.getNextSibling();
                } 
            } 
            kid = kid.getNextSibling();
        }

    }

    private String getText(Node node) 
    {
        StringBuffer sb = new StringBuffer();
        Node kid = node.getFirstChild();
        while (kid != null) {
            if (kid.getNodeType() == Node.TEXT_NODE) {
                sb.append(kid.getNodeValue());
            }
            kid = kid.getNextSibling();
        }
        return sb.toString();
    }

    public String getName()
    {
        return name;
    }

    public String getDescription()
    {
        return description;
    }

    public String getCoreRegex()
    {
        return coreRegex;
    }

    public String getCoreReservedRegex()
    {
        return coreReservedRegex;
    }

    public String getCoreParserName()
    {
        return coreParserName;
    }

    public String getFrontEndRegex()
    {
        return frontEndRegex;
    }

    public String getFrontEndReservedRegex()
    {
        return frontEndReservedRegex;
    }

    public String getFrontEndParserName()
    {
        return frontEndParserName;
    }
}

