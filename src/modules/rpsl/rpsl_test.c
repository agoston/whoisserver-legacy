#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "syntax_api.h"

const char person[] =
    "person:       Shane W. Kerr\n"
    "address:      RIPE NCC\n"
    "              Singel 258\n"
    "              1016 AB Amsterdam\n"
    "              The Netherlands\n"
    "phone:        +31 20 535 4444\n"
    "fax-no:       +31 20 545 4445\n"
    "e-mail:       shane@ripe.net\n"
    "nic-hdl:      SWK3-RIPE\n"
    "notify:       shane@ripe.net\n"
    "changed:      shane@ripe.net 20020819\n"
    "source:       RIPE";

const char route[] =
    "route:        193.0.0.0/21\n"
    "descr:        RIPE-NCC\n"
    "origin:       AS3333\n"
    "mnt-by:       RIPE-NCC-MNT\n"
    "changed:      GeertJan.deGroot@ripe.net 19960812\n"
    "changed:      ripe-dbm@ripe.net 19970822\n"
    "changed:      ripe-dbm@ripe.net 19970822\n"
    "changed:      OKolkman@ripe.net 19980102\n"
    "changed:      ripe-dbm@ripe.net 19980225\n"
    "changed:      joao@ripe.net 19980720\n"
    "changed:      joao@ripe.net 20000908\n"
    "source:       RIPE";

const char inetnum[] =
    "inetnum:      192.168.0.0 - 192.168.255.255\n"
    "netname:      IANA-CBLK-RESERVED1\n"
    "descr:        Class C address space for private internets\n"
    "descr:        See http://www.ripe.net/db/rfc1918.html for details\n"
    "country:      NL\n"
    "admin-c:      RFC1918-RIPE\n"
    "tech-c:       RFC1918-RIPE\n"
    "status:       ALLOCATED UNSPECIFIED\n"
    "remarks:      Country is really worldwide\n"
    "remarks:      This network should never be routed outside an enterprise\n"
    "remarks:      See RFC1918 for further information\n"
    "mnt-by:       RIPE-NCC-HM-MNT\n"
    "mnt-lower:    RIPE-NCC-HM-MNT\n"
    "changed:      rfc1918@ripe.net 20020129\n"
    "source:       RIPE";


int 
main ()
{
    rpsl_object_t *obj;
    gchar *key;
    GList *list, *p;
    rpsl_attr_t *attr;

    obj = rpsl_object_init(person);
    key = rpsl_object_get_key_value(obj);
    printf("key is %s\n", key);
    free(key);
    rpsl_object_delete(obj);

    obj = rpsl_object_init(route);
    key = rpsl_object_get_key_value(obj);
    printf("key is %s\n", key);
    free(key);

    printf("===========\n");
    list = rpsl_object_get_attr(obj, "changed");
    for (p=list; p != NULL; p = g_list_next(p)) 
    {
      attr = p->data;
      printf("%d: %s\n", rpsl_attr_get_ofs(attr), rpsl_attr_get_value(attr));
    }
    rpsl_attr_delete_list(list);

    printf("===========\n");
    list = rpsl_object_get_attr(obj, "changed");
    attr = list->data;
    rpsl_object_remove_attr(obj, rpsl_attr_get_ofs(attr), NULL);
//    rpsl_object_append_attr(obj, attr, NULL);
    rpsl_attr_delete_list(list);
    attr = rpsl_attr_init("changed: shane@ripe.net 20020408", NULL);
    rpsl_object_append_attr(obj, attr, NULL);
    list = rpsl_object_get_attr(obj, "changed");
    for (p=list; p != NULL; p = g_list_next(p)) 
    {
      attr = p->data;
      printf("%d: %s\n", rpsl_attr_get_ofs(attr), rpsl_attr_get_value(attr));
    }
    rpsl_attr_delete_list(list);
    rpsl_object_delete(obj);

    obj = rpsl_object_init(inetnum);
    key = rpsl_object_get_key_value(obj);
    printf("key is %s\n", key);
    free(key);
    rpsl_object_delete(obj);

    return 0;
}

