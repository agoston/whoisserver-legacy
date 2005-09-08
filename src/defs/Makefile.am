SUFFIXES = .java .class

.PHONY: syntax

noinst_DATA = AttributeSyntax.class RPSLSyntax.class ClassDef.class AttributeDef.class Query.class Defs.class

ClassDef.class: ClassDef.java
AttributeDef.class: AttributeDef.java
Query.class: Query.java
AttributeSyntax.class: AttributeSyntax.java
Defs.class: Defs.java
RPSLSyntax.class: RPSLSyntax.java

.PHONY: compile-classes

%.class: %.java
	@$(JAVAC) -d @abs_top_builddir@/src/defs -classpath @abs_top_srcdir@/src/defs:@abs_top_builddir@/src/defs:@abs_top_srcdir@/src/defs/xalan $^

compile-classes: $(noinst_DATA)

clean:
	rm -f *.class all @top_builddir@/src/modules/rpsl/*.o syntax alltodo

syntax:
	mkdir -p @abs_top_builddir@/src/modules/rpsl && \
	cd  @abs_top_builddir@/src/modules/rpsl && \
	RIPINCDIR="@abs_top_builddir@/src/include/" RIPLIBDIR="@abs_top_builddir@/src/" CC="$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)" CFLAGS="-I@abs_top_srcdir@/src/defs -I@abs_top_builddir@/src/defs $(CFLAGS) $(GLIB_CFLAGS) -I@abs_top_builddir@/src/include/" $(MAKE) $(AM_MAKEFLAGS) -f @abs_top_srcdir@/src/defs/Makefile.syntax && \
	touch syntax

all: compile-classes alltodo syntax

alltodo: AttributeSyntax.class \
         RPSLSyntax.class \
         Defs.class \
         ClassDef.class \
         AttributeDef.class \
         Query.class \
         attributes.xml \
         attributes.dtd \
         classes.xml \
         classes.dtd \
         syntax.xml \
         syntax.dtd
	./makedefsall && \
	touch alltodo
