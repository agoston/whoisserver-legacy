#!/bin/sed -f
#
# To be used by mirror reflector, this removes all references to person
# and role objects, replacing them with dummy objects
# $Id: noperson.sed,v 1.3 2004/12/10 16:37:46 can Exp $
#

# match person references
/^admin-c:/b alt
/^tech-c:/b alt
/^zone-c:/b alt
/^author:/ {
  :alt
  s/^\([^ ][^ ]*:  *\).*$/\1PP5000-RIPE/
  }

# mask person objects
/^person:/ {
  /^$/b
  i\
person:   Person Placeholder \
address:  Behind Every Network \
phone:    +31 32 3334356 \
e-mail:   bit-bucket@thishost.net \
nic-hdl:  PP5001-RIPE \
changed:  bit-bucket@ripe.net 19800101 \
source:   RIPE
  t loopbegin
}

# mask role objects
/^role:/ {
  /^$/b
  i\
role:     Role Placeholder \
address:  Behind Every Network \
phone:    +31 32 3334356 \
e-mail:   bit-bucket@thishost.net \
admin-c:  PP5000-RIPE \
tech-c:   PP5000-RIPE \
nic-hdl:  PP5001-RIPE \
changed:  bit-bucket@ripe.net 19800101 \
source:   RIPE
  t loopbegin
}

# loop that eats paragraphs
:loopbegin
/^person:/b loop
/^role:/ {
  :loop
  $! {
    N
    /\n$/! {
      s/.*\n//
      b loop
    }
  }
  s/.*\n//
  }
