#
#   ETA/OS - Device base class
#   Copyright (C) 2017  Michel Megens <dev@bietje.net>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

"""__NATIVE__
#include <etaos/stdio.h>
#include <etaos/device.h>
#include <etaos/vfs.h>
#include <etaos/unistd.h>
#include <etaos/error.h>
"""

__name__ = "device"

class Device(object):
        def __init__(self, name):
                self.path = "/dev/%s" % name
                self.desc = -1
                self.is_open = False

        def open(self):
                self.desc = dev_open(self, self.path)
                if self.desc >= 0:
                        self.is_open = True

        def read(self, num, addr = None):
                if self.is_open:
                        return dev_read(self.desc, num, addr)
                else:
                        return None


        def write(self, data, addr = None):
                if self.is_open:
                        return dev_write(self.desc, data, len(data), addr)
                else:
                        return None

        def close(self):
                dev_close(self, self.desc)
                self.is_open = False

        def write_list(self, ary, flt = False, addr = None):
                if not self.is_open:
                        return None
                return dev_write_list(self.desc, ary, len(ary), flt, addr)

        def read_list(self, num, flt = False, addr = False):
                if not self.is_open:
                        return None
                return dev_read_list(self.desc, num, flt, addr)

        def write_numeral(self, num, addr = None):
                if not self.is_open:
                        return None
                return dev_write_numeral(self.desc, num, addr)

        def read_numeral(self, addr = None, flt = False):
                if not self.is_open:
                        return None
                return dev_read_numeral(self.desc, flt, addr)

def dev_write_numeral(desc, num, addr):
        """__NATIVE__
        pPmObj_t desc, numeral, address;
        int fd;
        size_t addr, size;
        const void *buf;

        desc = NATIVE_GET_LOCAL(0);
        numeral = NATIVE_GET_LOCAL(1);
        address = NATIVE_GET_LOCAL(2);

        if(OBJ_GET_TYPE(numeral) == OBJ_TYPE_FLT) {
                buf = &((pPmFloat_t)numeral)->val;
                size = sizeof(float);
        } else {
                buf = &((pPmInt_t)numeral)->val;
                size = sizeof(int32_t);
        }

        fd = ((pPmInt_t)desc)->val;
        if(address != PM_NONE) {
                addr = ((pPmInt_t)address)->val;
                device_write(fd, buf, size, &addr);
        } else {
                device_write(fd, buf, size, NULL);
        }

        NATIVE_SET_TOS(PM_NONE);
        return PM_RET_OK;
        """
        pass

def dev_read_numeral(desc, flt, addr):
        """__NATIVE__
        pPmObj_t desc, numeral, address, flt;
        int fd, rv;
        size_t addr, size;
        uint8_t buf[8];
        float *f;
        int32_t *i;

        desc = NATIVE_GET_LOCAL(0);
        flt = NATIVE_GET_LOCAL(1);
        address = NATIVE_GET_LOCAL(2);

        fd = ((pPmInt_t)desc)->val;
        size = (flt == PM_TRUE) ? sizeof(float) : sizeof(int32_t);
        if(address != PM_NONE) {
                addr = ((pPmInt_t)address)->val;
                rv = device_read(fd, buf, size, &addr);
        } else {
                rv = device_read(fd, buf, size, NULL);
        }

        if(rv != -EOK) {
                NATIVE_SET_TOS(PM_NONE);
                return PM_RET_OK;
        }

        if(flt == PM_TRUE) {
                f = (float*)buf;
                float_new(*f, &numeral);
        } else {
                i = (int32_t*)buf;
                int_new(*i, &numeral);
        }

        NATIVE_SET_TOS(numeral);
        return PM_RET_OK;
        """
        pass

def dev_read_list(desc, num, flt, addr):
        """__NATIVE__
        pPmObj_t pdesc, pary, plength, paddr, obj, pflt;
        int fd, idx, rv;
        size_t address, length, size;
        float *farry;
        int32_t *iarry;
        void *buf;

        pdesc = NATIVE_GET_LOCAL(0);
        plength = NATIVE_GET_LOCAL(1);
        pflt = NATIVE_GET_LOCAL(2);
        paddr = NATIVE_GET_LOCAL(3);

        fd = ((pPmInt_t)pdesc)->val;
        length = ((pPmInt_t)plength)->val;
        list_new(&pary);

        if(pflt == PM_TRUE) {
                size = length * sizeof(float);
        } else {
                size = length * sizeof(int32_t);
        }

        iarry = buf = kzalloc(size);
        farry = buf;

        if(paddr != PM_NONE) {
                address = ((pPmInt_t)paddr)->val;
                rv = device_read(fd, buf, size, &address);
        } else {
                rv = device_read(fd, buf, size, NULL);
        }

        if(rv != -EOK) {
                kfree(buf);
                NATIVE_SET_TOS(pary);
                return PM_RET_OK;
        }

        for(idx = 0; idx < length; idx++) {
                pflt == PM_TRUE ? float_new(farry[idx], &obj) :
                                  int_new(iarry[idx], &obj);
                list_append(pary, obj);
        }

        kfree(buf);
        NATIVE_SET_TOS(pary);
        return PM_RET_OK;
        """
        pass

def dev_write_list(desc, ary, length, flt, addr):
        """__NATIVE__
        pPmObj_t pdesc, pary, plength, paddr, obj, retval, pflt;
        int fd, idx, rv;
        size_t address, length;
        float *farry;
        int32_t *iarry;
        const void *buf;

        pdesc = NATIVE_GET_LOCAL(0);
        pary = NATIVE_GET_LOCAL(1);
        plength = NATIVE_GET_LOCAL(2);
        pflt = NATIVE_GET_LOCAL(3);
        paddr = NATIVE_GET_LOCAL(4);

        fd = ((pPmInt_t)pdesc)->val;
        length = ((pPmInt_t)plength)->val;

        if(pflt == PM_TRUE) {
                farry = kzalloc(length * sizeof(float));
                for(idx = 0; idx < length; idx++) {
                        list_getItem(pary, idx, &obj);
                        farry[idx] = ((pPmFloat_t)obj)->val;
                }

                length = length * sizeof(float);
                buf = farry;
        } else {
                iarry = kzalloc(length * sizeof(int32_t));
                for(idx = 0; idx < length; idx++) {
                        list_getItem(pary, idx, &obj);
                        iarry[idx] = ((pPmInt_t)obj)->val;
                }

                length = length * sizeof(float);
                buf = iarry;
        }

        if(paddr != PM_NONE) {
                address = ((pPmInt_t)paddr)->val;
                rv = device_write(fd, buf, length, &address);
        } else {
                rv = device_write(fd, buf, length, NULL);
        }

        int_new(rv, &retval);
        NATIVE_SET_TOS(retval);
        return PM_RET_OK;
        """
        pass

def dev_open(self, path):
        """__NATIVE__
        pPmObj_t path, desc;
        const char *filepath;
        int fd;
        PmReturn_t retval;

        path = NATIVE_GET_LOCAL(1);
        filepath = (char*)((pPmString_t)path)->val;
        fd = open(filepath, _FDEV_SETUP_RW);
        retval = int_new(fd, &desc);
        NATIVE_SET_TOS(desc);
        return retval;
        """
        pass

def dev_close(self, fd):
        """__NATIVE__
        pPmObj_t desc;
        int fd;

        desc = NATIVE_GET_LOCAL(1);

        fd = (int)((pPmInt_t)desc)->val;
        if(fd > 0)
                fd = close(fd);

        NATIVE_SET_TOS(PM_NONE);
        return PM_RET_OK;
        """
        pass

def dev_write(desc, data, num, addr):
        """__NATIVE__
        pPmObj_t desc, data, num, addr, retval;
        int rv, fd;
        void *buf;
        size_t address, nbytes;

        desc = NATIVE_GET_LOCAL(0);
        data = NATIVE_GET_LOCAL(1);
        num = NATIVE_GET_LOCAL(2);
        addr = NATIVE_GET_LOCAL(3);

        fd = ((pPmInt_t)desc)->val;
        buf = ((pPmString_t)data)->val;
        nbytes = ((pPmInt_t)num)->val + 1;

        if(addr != PM_NONE) {
                address = ((pPmInt_t)addr)->val;
                rv = device_write(fd, buf, nbytes, &address);
        } else {
                rv = device_write(fd, buf, nbytes, NULL);
        }

        int_new(rv, &retval);
        NATIVE_SET_TOS(retval);
        return PM_RET_OK;
        """
        pass

def dev_read(desc, num, addr):
        """__NATIVE__
        pPmObj_t desc, data, num, addr;
        int fd, rv;
        char *buf;
        size_t address, nbytes;

        desc = NATIVE_GET_LOCAL(0);
        num = NATIVE_GET_LOCAL(1);
        addr = NATIVE_GET_LOCAL(2);

        fd = ((pPmInt_t)desc)->val;
        nbytes = ((pPmInt_t)num)->val;
        buf = kzalloc(nbytes + 1);

        if(addr != PM_NONE) {
                address = ((pPmInt_t)addr)->val;
                rv = device_read(fd, buf, nbytes, &address);
        } else {
                rv = device_read(fd, buf, nbytes, NULL);
        }

        if(rv != -EOK) {
                NATIVE_SET_TOS(PM_NONE);
                kfree(buf);
                return PM_RET_OK;
        }

        string_new((const char*)&buf, &data);
        kfree(buf);
        NATIVE_SET_TOS(data);
        return PM_RET_OK;
        """
        pass
