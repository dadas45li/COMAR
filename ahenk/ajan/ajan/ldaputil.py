#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (C) 2007, TUBITAK/UEKAE
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version. Please read the COPYING file.
#

import ldap

import ajan.config


class LdapClass:
    def fromEntry(self, attr):
        for varname, attrname, valuetype, default in self.entries:
            value = attr.get(attrname, None)
            if value:
                if valuetype == int:
                    val = int(value[0])
                elif valuetype == str:
                    val = unicode(value[0])
                else:
                    val = value
            else:
                val = default
            setattr(self, varname, val)
    
    def toEntry(self):
        attr = {}
        for item in self.entries:
            val = getattr(self, item[0])
            if item[2] == int:
                val = [str(val)]
            elif item[2] == str:
                val = [val]
            attr[item[1]] = val
        return attr


class Connection:
    def __init__(self):
        conf = ajan.config.ldap
        conn = ldap.open(conf.uri)
        conn.simple_bind_s(conf.bind_dn, conf.bind_password)
        self.conn = conn
    
    def close(self):
        self.conn.unbind_s()
        self.conn = None
    
    def search_computer(self):
        return self.conn.search_s(
            ajan.config.computer_dn,
            ldap.SCOPE_BASE,
            "objectClass=pardusComputer"
        )
        return ret
    
    def search_ou(self, unit):
        return self.conn.search_s(
            unit,
            ldap.SCOPE_BASE,
            "objectClass=organizationalUnit"
        )